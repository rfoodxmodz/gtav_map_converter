/*
This tool was created to convert the Menyoo, Map Editor
And Simple Trainer maps into (.ini files) supported by the Scorpion's Menu on PS4
The maps referred to here are Objects, Peds, and Vehicles
Not ymap files which allow adding new buildings and other items
This tool can be developed to support other maps for other menus.

Coded by @rfoodxmodz
*/
#pragma warning(disable:4996);
#include <iostream>
#include "libs/pugixml/src/pugixml.hpp"
#include <unordered_set>
#include <tchar.h>
#include <windows.h>
#pragma comment(lib, "shlwapi.lib")
#include <Shlwapi.h>
#include "config.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif


/*variables*/
struct Vector3 {
	float x;
	float y;
	float z;
};
struct RgbS {
	int R, G, B;
};
int vehicles_count;
int peds_count;
int objects_count;
bool is_map_type;
char szFile[100];


std::wstring ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

std::string GetExeFileName()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
int main()
{
	printf("Scorpion's Maps Converter (Coded by @rfoodxmodz)\n");
	char nbuff[255];
	char fbuff[255];
	OPENFILENAMEA ofn;
	// open a file name
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileNameA(&ofn);

	strcpy_s(nbuff, ofn.lpstrFile);
	PathStripPathA(ofn.lpstrFile);
	int len, i;
	for (len = strlen(ofn.lpstrFile); len >= 0 && ofn.lpstrFile[len] != '.'; len--)
		;

	char* str = (char*)malloc(len + 1);
	
	for (i = 0; i < len; i++)
		str[i] = ofn.lpstrFile[i];

	str[i] = '\0';
	strcpy_s(fbuff, str);
	
	char buffer[MAX_PATH] = { 0 };
	char fullPath[MAX_PATH];
	char driveLetter[3];
	char directory[MAX_PATH];
	char FinalPath[MAX_PATH];
	GetModuleFileNameA(NULL, fullPath, MAX_PATH);
	_splitpath(fullPath, driveLetter, directory, NULL, NULL);
	sprintf_s(FinalPath, "%s%s", driveLetter, directory);
	
	char vbuff[255];
	strcpy(vbuff, FinalPath);
	strcat(vbuff, fbuff);
	strcat(vbuff, ".ini");
	printf("File Name: %s\n", nbuff);
	printf("Converting the map ...\n");
	objects_count = 0;
	vehicles_count = 0;
	pugi::xml_document docx;
	pugi::xml_parse_result result = docx.load_file(nbuff);
	if (!result) {
		printf("failed to open\n");
		return -1;
	}
	ini_table_s* config2 = ini_table_create();
	ini_section_s* section;
	ini_entry_s* entry;
	pugi::xml_node nodeRoot = docx.child("SpoonerPlacements").child("Placement");
	uint32_t model_hash = nodeRoot.child("ModelHash").text().as_uint();
	pugi::xml_node placement = nodeRoot.child("Placement");
	pugi::xml_node ReferenceCoords = docx.child("SpoonerPlacements");
	if (!ReferenceCoords) {
		pugi::xml_node nodeRoot_mapEditorMain = docx.child("Map");
		pugi::xml_node nodeRoot_mapEditor = docx.child("Map").child("Objects").child("MapObject");
		if (!is_map_type) {
			printf("Map Editor: Maps Detected\n");
			is_map_type = 1;
		}
		//printf("Type: %s\n", nodeRoot_mapEditor.child("Type").text().as_string());
		const char* X = nodeRoot_mapEditorMain.child("Metadata").child("TeleportPoint").child("X").text().as_string();
		const char* Y = nodeRoot_mapEditorMain.child("Metadata").child("TeleportPoint").child("Y").text().as_string();
		const char* Z = nodeRoot_mapEditorMain.child("Metadata").child("TeleportPoint").child("Z").text().as_string();
		section = _ini_section_create(config2, "TELEPORT");
		entry = _ini_entry_create(section, "X", X == "" ? "NIL" : X);
		entry = _ini_entry_create(section, "Y", Y == "" ? "NIL" : Y);
		entry = _ini_entry_create(section, "Z", Z == "" ? "NIL" : Z);
		ini_table_write_to_file(config2, vbuff);
		for (pugi::xml_node nodeEntity = nodeRoot_mapEditor; nodeEntity; nodeEntity = nodeEntity.next_sibling("MapObject"))
		{
			if (!strcmp(nodeEntity.child("Type").text().as_string(), "Prop")) {
				section = _ini_section_create(config2, "OBJECT-E");
				entry = _ini_entry_create(section, "MODEL", nodeEntity.child("Hash").text().as_string());
				entry = _ini_entry_create(section, "X", nodeEntity.child("Position").child("X").text().as_string());
				entry = _ini_entry_create(section, "Y", nodeEntity.child("Position").child("Y").text().as_string());
				entry = _ini_entry_create(section, "Z", nodeEntity.child("Position").child("Z").text().as_string());
				entry = _ini_entry_create(section, "RX", nodeEntity.child("Rotation").child("X").text().as_string());
				entry = _ini_entry_create(section, "RY", nodeEntity.child("Rotation").child("Y").text().as_string());
				entry = _ini_entry_create(section, "RZ", nodeEntity.child("Rotation").child("Z").text().as_string());
				entry = _ini_entry_create(section, "QX", nodeEntity.child("Quaternion").child("X").text().as_string() == "" ? "NIL" : nodeEntity.child("Quaternion").child("X").text().as_string());
				entry = _ini_entry_create(section, "QY", nodeEntity.child("Quaternion").child("Y").text().as_string() == "" ? "NIL" : nodeEntity.child("Quaternion").child("Y").text().as_string());
				entry = _ini_entry_create(section, "QZ", nodeEntity.child("Quaternion").child("Z").text().as_string() == "" ? "NIL" : nodeEntity.child("Quaternion").child("Z").text().as_string());
				entry = _ini_entry_create(section, "QW", nodeEntity.child("Quaternion").child("W").text().as_string() == "" ? "NIL" : nodeEntity.child("Quaternion").child("W").text().as_string());
				entry = _ini_entry_create(section, "DYNAMIC", nodeEntity.child("Dynamic").text().as_string());
				ini_table_write_to_file(config2, vbuff);
				objects_count++;
			}
			if (!strcmp(nodeEntity.child("Type").text().as_string(), "Vehicle")) {
				section = _ini_section_create(config2, "VEHICLE-E");
				entry = _ini_entry_create(section, "MODEL", nodeEntity.child("Hash").text().as_string());
				entry = _ini_entry_create(section, "X", nodeEntity.child("Position").child("X").text().as_string());
				entry = _ini_entry_create(section, "Y", nodeEntity.child("Position").child("Y").text().as_string());
				entry = _ini_entry_create(section, "Z", nodeEntity.child("Position").child("Z").text().as_string());
				entry = _ini_entry_create(section, "RX", nodeEntity.child("Rotation").child("X").text().as_string());
				entry = _ini_entry_create(section, "RY", nodeEntity.child("Rotation").child("Y").text().as_string());
				entry = _ini_entry_create(section, "RZ", nodeEntity.child("Rotation").child("Z").text().as_string());
				entry = _ini_entry_create(section, "QX", nodeEntity.child("Quaternion").child("X").text().as_string() == "" ? "NIL" : nodeEntity.child("Quaternion").child("X").text().as_string());
				entry = _ini_entry_create(section, "QY", nodeEntity.child("Quaternion").child("Y").text().as_string() == "" ? "NIL" : nodeEntity.child("Quaternion").child("Y").text().as_string());
				entry = _ini_entry_create(section, "QZ", nodeEntity.child("Quaternion").child("Z").text().as_string() == "" ? "NIL" : nodeEntity.child("Quaternion").child("Z").text().as_string());
				entry = _ini_entry_create(section, "QW", nodeEntity.child("Quaternion").child("W").text().as_string() == "" ? "NIL" : nodeEntity.child("Quaternion").child("W").text().as_string());
				entry = _ini_entry_create(section, "DYNAMIC", nodeEntity.child("Dynamic").text().as_string());
				entry = _ini_entry_create(section, "SIRENS_ACTIVE", nodeEntity.child("SirensActive").text().as_string());
				entry = _ini_entry_create(section, "PRIMARY_COLOR", nodeEntity.child("PrimaryColor").text().as_string());
				entry = _ini_entry_create(section, "SECONDARY_COLOR", nodeEntity.child("SecondaryColor").text().as_string());
				ini_table_write_to_file(config2, vbuff);
				vehicles_count++;
			}
		}
	}
	else {
		pugi::xml_node ReferenceCoords = docx.child("SpoonerPlacements").child("ReferenceCoords");
		section = _ini_section_create(config2, "TELEPORT");
		if ((ReferenceCoords.child("X"))) {
			entry = _ini_entry_create(section, "X", ReferenceCoords.child("X").text().as_string());
			entry = _ini_entry_create(section, "Y", ReferenceCoords.child("Y").text().as_string());
			entry = _ini_entry_create(section, "Z", ReferenceCoords.child("Z").text().as_string());
			ini_table_write_to_file(config2, vbuff);
		}
		else {
			auto nodeEntityPosRot = nodeRoot.child("PositionRotation");
			entry = _ini_entry_create(section, "X", nodeEntityPosRot.child("X").text().as_string());
			entry = _ini_entry_create(section, "Y", nodeEntityPosRot.child("Y").text().as_string());
			entry = _ini_entry_create(section, "Z", nodeEntityPosRot.child("Z").text().as_string());
			ini_table_write_to_file(config2, vbuff);
		}

		for (pugi::xml_node nodeEntity = nodeRoot; nodeEntity; nodeEntity = nodeEntity.next_sibling("Placement"))
		{
			uint32_t model_hash = nodeEntity.child("ModelHash").text().as_uint();
			uint32_t entity_type = nodeEntity.child("Type").text().as_int();
			bool is_dynamic = nodeEntity.child("Dynamic").text().as_bool();
			if (entity_type == 3) { //OBJECT
				nodeEntity.child("FrozenPos").text().as_bool();
				auto nodeEntityPosRot = nodeEntity.child("PositionRotation");
				nodeEntity.child("HasGravity").text().as_bool();
				auto nodePropStuff = nodeEntity.child("ObjectProperties");
				section = _ini_section_create(config2, "OBJECT");
				entry = _ini_entry_create(section, "MODEL", nodeEntity.child("ModelHash").text().as_string());
				entry = _ini_entry_create(section, "X", nodeEntityPosRot.child("X").text().as_string());
				entry = _ini_entry_create(section, "Y", nodeEntityPosRot.child("Y").text().as_string());
				entry = _ini_entry_create(section, "Z", nodeEntityPosRot.child("Z").text().as_string());
				entry = _ini_entry_create(section, "RX", nodeEntityPosRot.child("Pitch").text().as_string());
				entry = _ini_entry_create(section, "RY", nodeEntityPosRot.child("Roll").text().as_string());
				entry = _ini_entry_create(section, "RZ", nodeEntityPosRot.child("Yaw").text().as_string());
				entry = _ini_entry_create(section, "DYNAMIC", nodeEntity.child("Dynamic").text().as_string());
				entry = _ini_entry_create(section, "FROZENPOS", nodeEntity.child("FrozenPos").text().as_string());
				entry = _ini_entry_create(section, "INVINCIBLE", nodeEntity.child("IsInvincible").text().as_string());
				entry = _ini_entry_create(section, "VISIBLE", nodeEntity.child("IsVisible").text().as_string());
				entry = _ini_entry_create(section, "HAS_GRAVITY", nodeEntity.child("HasGravity").text().as_string());
				entry = _ini_entry_create(section, "OPACITY", nodeEntity.child("OpacityLevel").text().as_string());
				entry = _ini_entry_create(section, "MAX_HEALTH", nodeEntity.child("MaxHealth").text().as_string());
				entry = _ini_entry_create(section, "HEALTH", nodeEntity.child("Health").text().as_string());
				entry = _ini_entry_create(section, "LOD_DISTANCE", nodeEntity.child("LodDistance").text().as_string());
				entry = _ini_entry_create(section, "TEXTURE_VARIATION", nodePropStuff.child("TextureVariation").text().as_string() == "" ? "0" : nodePropStuff.child("TextureVariation").text().as_string());
				ini_table_write_to_file(config2, vbuff);
				objects_count++;
			}
			if (entity_type == 2) { //VEHICLE
				auto nodeEntityPosRot = nodeEntity.child("PositionRotation");
				auto nodeVehicleStuff = nodeEntity.child("VehicleProperties");
				auto nodeVehicleColours = nodeVehicleStuff.child("Colours");
				// Neons
				auto nodeVehicleNeons = nodeVehicleStuff.child("Neons");
				section = _ini_section_create(config2, "VEHICLE");
				entry = _ini_entry_create(section, "MODEL", nodeEntity.child("ModelHash").text().as_string());
				entry = _ini_entry_create(section, "X", nodeEntityPosRot.child("X").text().as_string());
				entry = _ini_entry_create(section, "Y", nodeEntityPosRot.child("Y").text().as_string());
				entry = _ini_entry_create(section, "Z", nodeEntityPosRot.child("Z").text().as_string());
				entry = _ini_entry_create(section, "RX", nodeEntityPosRot.child("Pitch").text().as_string());
				entry = _ini_entry_create(section, "RY", nodeEntityPosRot.child("Roll").text().as_string());
				entry = _ini_entry_create(section, "RZ", nodeEntityPosRot.child("Yaw").text().as_string());
				entry = _ini_entry_create(section, "DYNAMIC", nodeEntity.child("Dynamic").text().as_string());
				entry = _ini_entry_create(section, "FROZENPOS", nodeEntity.child("FrozenPos").text().as_string());
				entry = _ini_entry_create(section, "INVINCIBLE", nodeEntity.child("IsInvincible").text().as_string());
				entry = _ini_entry_create(section, "VISIBLE", nodeEntity.child("IsVisible").text().as_string());
				entry = _ini_entry_create(section, "HAS_GRAVITY", nodeEntity.child("HasGravity").text().as_string());
				entry = _ini_entry_create(section, "OPACITY", nodeEntity.child("OpacityLevel").text().as_string());
				entry = _ini_entry_create(section, "MAX_HEALTH", nodeEntity.child("MaxHealth").text().as_string());
				entry = _ini_entry_create(section, "HEALTH", nodeEntity.child("Health").text().as_string());
				entry = _ini_entry_create(section, "LOD_DISTANCE", nodeEntity.child("LodDistance").text().as_string());
				entry = _ini_entry_create(section, "PRIMARY", nodeVehicleColours.child("Primary").text().as_string());
				entry = _ini_entry_create(section, "SECONDARY", nodeVehicleColours.child("Secondary").text().as_string());
				if (nodeVehicleColours.child("Pearl"))
					entry = _ini_entry_create(section, "PEARL", nodeVehicleColours.child("Pearl").text().as_string());
				entry = _ini_entry_create(section, "RIM", nodeVehicleColours.child("Rim").text().as_string());
				entry = _ini_entry_create(section, "MOD1_A", nodeVehicleColours.child("Mod1_a").text().as_string());
				entry = _ini_entry_create(section, "MOD1_B", nodeVehicleColours.child("Mod1_b").text().as_string());
				entry = _ini_entry_create(section, "MOD1_C", nodeVehicleColours.child("Mod1_c").text().as_string());
				entry = _ini_entry_create(section, "MOD2_A", nodeVehicleColours.child("Mod2_a").text().as_string());
				entry = _ini_entry_create(section, "MOD2_B", nodeVehicleColours.child("Mod2_b").text().as_string());
				entry = _ini_entry_create(section, "ISPRIMARYCOLOURCUSTOM", nodeVehicleColours.child("IsPrimaryColourCustom").text().as_string());
				entry = _ini_entry_create(section, "ISSECONDARYCOLOURCUSTOM", nodeVehicleColours.child("IsSecondaryColourCustom").text().as_string());
				if (nodeVehicleColours.child("Cust1_R"))
					entry = _ini_entry_create(section, "CUST1_R", nodeVehicleColours.child("Cust1_R").text().as_string());
				if (nodeVehicleColours.child("Cust1_G"))
					entry = _ini_entry_create(section, "CUST1_G", nodeVehicleColours.child("Cust1_G").text().as_string());
				if (nodeVehicleColours.child("Cust1_B"))
					entry = _ini_entry_create(section, "CUST1_B", nodeVehicleColours.child("Cust1_B").text().as_string());
				if (nodeVehicleColours.child("Cust2_R"))
					entry = _ini_entry_create(section, "CUST2_R", nodeVehicleColours.child("Cust2_R").text().as_string());
				if (nodeVehicleColours.child("Cust2_G"))
					entry = _ini_entry_create(section, "CUST2_G", nodeVehicleColours.child("Cust2_G").text().as_string());
				if (nodeVehicleColours.child("Cust2_B"))
					entry = _ini_entry_create(section, "CUST2_B", nodeVehicleColours.child("Cust2_B").text().as_string());
				if (nodeVehicleColours.child("tyreSmoke_R"))
					entry = _ini_entry_create(section, "TYRESMOKE_R", nodeVehicleColours.child("tyreSmoke_R").text().as_string());
				if (nodeVehicleColours.child("tyreSmoke_G"))
					entry = _ini_entry_create(section, "TYRESMOKE_G", nodeVehicleColours.child("tyreSmoke_G").text().as_string());
				if (nodeVehicleColours.child("tyreSmoke_B"))
					entry = _ini_entry_create(section, "TYRESMOKE_B", nodeVehicleColours.child("tyreSmoke_B").text().as_string());
				if (nodeVehicleColours.child("LrInterior"))
					entry = _ini_entry_create(section, "LRINTERIOR", nodeVehicleColours.child("LrInterior").text().as_string());
				if (nodeVehicleColours.child("LrDashboard"))
					entry = _ini_entry_create(section, "LRDASHBOARD", nodeVehicleColours.child("LrDashboard").text().as_string());
				if (nodeVehicleColours.child("LrXenonHeadlights"))
					entry = _ini_entry_create(section, "LRXENONHEADLIGHTS", nodeVehicleColours.child("LrXenonHeadlights").text().as_string());
				if (nodeVehicleStuff.child("NumberPlateText"))
					entry = _ini_entry_create(section, "NUMBER_PLATETEXT", nodeVehicleStuff.child("NumberPlateText").text().as_string());
				if (nodeVehicleStuff.child("NumberPlateIndex"))
					entry = _ini_entry_create(section, "NUMBER_PLATEINDEX", nodeVehicleStuff.child("NumberPlateIndex").text().as_string());
				if (nodeVehicleStuff.child("WheelType"))
					entry = _ini_entry_create(section, "WHEEL_TYPE", nodeVehicleStuff.child("WheelType").text().as_string());
				if (nodeVehicleStuff.child("WindowTint"))
					entry = _ini_entry_create(section, "WINDOW_TINT", nodeVehicleStuff.child("WindowTint").text().as_string());
				if (nodeVehicleStuff.child("BulletProofTyres"))
					entry = _ini_entry_create(section, "BULLET_PROOF_TYRES", nodeVehicleStuff.child("BulletProofTyres").text().as_string());
				if (nodeVehicleStuff.child("DirtLevel"))
					entry = _ini_entry_create(section, "DIRT_LEVEL", nodeVehicleStuff.child("DirtLevel").text().as_string());
				if (nodeVehicleStuff.child("PaintFade"))
					entry = _ini_entry_create(section, "PAINT_FADE", nodeVehicleStuff.child("PaintFade").text().as_string());
				if (nodeVehicleStuff.child("RoofState"))
					entry = _ini_entry_create(section, "ROOF_STATE", nodeVehicleStuff.child("RoofState").text().as_string());
				if (nodeVehicleStuff.child("SirenActive"))
					entry = _ini_entry_create(section, "SIREN_ACTIVE", nodeVehicleStuff.child("SirenActive").text().as_string());
				if (nodeVehicleStuff.child("EngineOn"))
					entry = _ini_entry_create(section, "ENGINE_ON", nodeVehicleStuff.child("EngineOn").text().as_string());
				if (nodeVehicleStuff.child("EngineHealth"))
					entry = _ini_entry_create(section, "ENGINE_HEALTH", nodeVehicleStuff.child("EngineHealth").text().as_string());
				if (nodeVehicleStuff.child("LightsOn"))
					entry = _ini_entry_create(section, "LIGHTSON", nodeVehicleStuff.child("LightsOn").text().as_string());
				if (nodeVehicleStuff.child("IsRadioLoud"))
					entry = _ini_entry_create(section, "IS_RADIOLOUD", nodeVehicleStuff.child("IsRadioLoud").text().as_string());
				if (nodeVehicleStuff.child("LockStatus"))
					entry = _ini_entry_create(section, "LOCKSTATUS", nodeVehicleStuff.child("LockStatus").text().as_string());
				entry = _ini_entry_create(section, "NEON_LEFT", nodeVehicleNeons.child("Left").text().as_string());
				entry = _ini_entry_create(section, "NEON_RIGHT", nodeVehicleNeons.child("Right").text().as_string());
				entry = _ini_entry_create(section, "NEON_FRONT", nodeVehicleNeons.child("Front").text().as_string());
				entry = _ini_entry_create(section, "NEON_BACK", nodeVehicleNeons.child("Back").text().as_string());
				entry = _ini_entry_create(section, "NEON_R", nodeVehicleNeons.child("R").text().as_string());
				entry = _ini_entry_create(section, "NEON_G", nodeVehicleNeons.child("G").text().as_string());
				entry = _ini_entry_create(section, "NEON_B", nodeVehicleNeons.child("B").text().as_string());
				if (nodeVehicleStuff.child("WheelsInvisible"))
					entry = _ini_entry_create(section, "WHEELS_INVISIBLE", nodeVehicleStuff.child("WheelsInvisible").text().as_string());
				if (strlen(nodeVehicleStuff.child("EngineSoundName").text().as_string()))
					entry = _ini_entry_create(section, "ENGINE_SOUND_NAME", nodeVehicleStuff.child("EngineSoundName").text().as_string());
				if (nodeVehicleStuff.child("RpmMultiplier"))
					entry = _ini_entry_create(section, "RPM_MULTIPLIER", nodeVehicleStuff.child("RpmMultiplier").text().as_string());
				if (nodeVehicleStuff.child("TorqueMultiplier"))
					entry = _ini_entry_create(section, "TORQUE_MULTIPLIER", nodeVehicleStuff.child("TorqueMultiplier").text().as_string());
				if (nodeVehicleStuff.child("MaxSpeed"))
					entry = _ini_entry_create(section, "MAX_SPEED", nodeVehicleStuff.child("MaxSpeed").text().as_string());
				if (nodeVehicleStuff.child("HeadlightIntensity"))
					entry = _ini_entry_create(section, "HEADLIGHT_INTENSITY", nodeVehicleStuff.child("HeadlightIntensity").text().as_string());
				if (nodeVehicleStuff.child("Livery"))
					entry = _ini_entry_create(section, "LIVERY", nodeVehicleStuff.child("Livery").text().as_string());

				ini_table_write_to_file(config2, vbuff);
				vehicles_count++;

			}
			if (entity_type == 1) { //PED
				nodeEntity.child("FrozenPos").text().as_bool();
				auto nodeEntityPosRot = nodeEntity.child("PositionRotation");
				nodeEntity.child("HasGravity").text().as_bool();
				auto nodePropStuff = nodeEntity.child("PedProperties");
				section = _ini_section_create(config2, "PED");
				entry = _ini_entry_create(section, "MODEL", nodeEntity.child("ModelHash").text().as_string());
				entry = _ini_entry_create(section, "X", nodeEntityPosRot.child("X").text().as_string());
				entry = _ini_entry_create(section, "Y", nodeEntityPosRot.child("Y").text().as_string());
				entry = _ini_entry_create(section, "Z", nodeEntityPosRot.child("Z").text().as_string());
				entry = _ini_entry_create(section, "RX", nodeEntityPosRot.child("Pitch").text().as_string());
				entry = _ini_entry_create(section, "RY", nodeEntityPosRot.child("Roll").text().as_string());
				entry = _ini_entry_create(section, "RZ", nodeEntityPosRot.child("Yaw").text().as_string());
				entry = _ini_entry_create(section, "DYNAMIC", nodeEntity.child("Dynamic").text().as_string());
				entry = _ini_entry_create(section, "FROZENPOS", nodeEntity.child("FrozenPos").text().as_string());
				entry = _ini_entry_create(section, "INVINCIBLE", nodeEntity.child("IsInvincible").text().as_string());
				entry = _ini_entry_create(section, "VISIBLE", nodeEntity.child("IsVisible").text().as_string());
				entry = _ini_entry_create(section, "HAS_GRAVITY", nodeEntity.child("HasGravity").text().as_string());
				entry = _ini_entry_create(section, "OPACITY", nodeEntity.child("OpacityLevel").text().as_string());
				entry = _ini_entry_create(section, "MAX_HEALTH", nodeEntity.child("MaxHealth").text().as_string());
				entry = _ini_entry_create(section, "HEALTH", nodeEntity.child("Health").text().as_string());
				entry = _ini_entry_create(section, "LOD_DISTANCE", nodeEntity.child("LodDistance").text().as_string());
				entry = _ini_entry_create(section, "ISONFIRE", nodeEntity.child("IsOnFire").text().as_string());
				entry = _ini_entry_create(section, "IS_STILL", nodePropStuff.child("IsStill").text().as_string());
				entry = _ini_entry_create(section, "CAN_RAGDOLL", nodePropStuff.child("CanRagdoll").text().as_string());
				entry = _ini_entry_create(section, "HAS_SHORT_HEIGHT", nodePropStuff.child("HasShortHeight").text().as_string());
				entry = _ini_entry_create(section, "ARMOUR", nodePropStuff.child("Armour").text().as_string());
				entry = _ini_entry_create(section, "CURRENT_WEAPON", nodePropStuff.child("CurrentWeapon").text().as_string());
				entry = _ini_entry_create(section, "RELATIONSHIP_GROUPALTERED", nodePropStuff.child("RelationshipGroupAltered").text().as_string());
				entry = _ini_entry_create(section, "RELATIONSHIP_GROUP", nodePropStuff.child("RelationshipGroup").text().as_string());
				entry = _ini_entry_create(section, "SCENARIO_ACTIVE", nodePropStuff.child("ScenarioActive").text().as_string());
				entry = _ini_entry_create(section, "SCENARIO_NAME", nodePropStuff.child("ScenarioName").text().as_string());
				entry = _ini_entry_create(section, "ANIM_ACTIVE", nodePropStuff.child("AnimActive").text().as_string());
				entry = _ini_entry_create(section, "ANIM_DICT", nodePropStuff.child("AnimDict").text().as_string() == "" ? "NIL" : nodePropStuff.child("AnimDict").text().as_string());
				entry = _ini_entry_create(section, "ANIM_NAME", nodePropStuff.child("AnimName").text().as_string() == "" ? "NIL" : nodePropStuff.child("AnimName").text().as_string());
				auto nodeEntityAttachment = nodeEntity.child("Attachment");
				entry = _ini_entry_create(section, "IS_ATTACHED", nodeEntityAttachment.child("isAttached").text().as_string());
				//entry = _ini_entry_create(section, "IS_STILL", nodePropStuff.child("IsStill").text().as_string() == "" ? "0" : nodePropStuff.child("TextureVariation").text().as_string());
				ini_table_write_to_file(config2, vbuff);
				peds_count++;
			}



		}
	}
	if (config2 != NULL)
		ini_table_destroy(config2);

	printf("-------------------------------------------------------------------------\n");
	printf("Objects Count: %i\n", objects_count);
	printf("Vehicles Count: %i\n", vehicles_count);
	printf("Peds Count: %i\n", peds_count);
	printf("Generated File Path: %s\n", vbuff);
	printf("The map has been converted successfully.\n\n");
	
	std::cin.ignore();
}