#include <jni.h>

#include "CustomFunctions.h"
#include "minecraftpe/client/MinecraftClient.h"
#include "minecraftpe/entity/player/Player.h"
#include "minecraftpe/network/protocol/MovePlayerPacket.h"
#include "minecraftpe/network/protocol/TextPacket.h"
#include "minecraftpe/network/protocol/SetPlayerGameTypePacket.h"
#include "minecraftpe/network/protocol/DisconnectPacket.h"
#include "hook/hook.h"
#include "access.h"

static MinecraftClient* mc = 0;
static void** Player_vtable = (void**)dlsym(RTLD_DEFAULT, "_ZTV6Player");

static const std::string TP_KEY = "@TELEPORT";
static bool TP_FLAG = false;

static const std::string PRIVATE_KEY = "@PRIVATE_SENDCHAT";
static const std::string GAMEMODE_KEY = "@GAMEMODE";
static const std::string DISCONNECT_KEY = "@DISCONNECT";

static bool operator==(Vec3& vec1, Vec3& vec2) {
	return vec1.x == vec2.x && vec1.y == vec2.y && vec1.z == vec2.z;
}

static bool isPlayer(Entity* entity) {
	return (access(void**, entity, 0))[0x14C] == Player_vtable[0x154];
}

static void(*MinecraftClient$init_real)(MinecraftClient*);
static void MinecraftClient$init_hook(MinecraftClient* $this) {
	MinecraftClient$init_real($this);
	mc = $this;
}

static void(*Gui$displayClientMessage_real)(Gui*, const std::string&);
static void Gui$displayClientMessage_hook(Gui* $this, const std::string& msg) {
	if (msg == TP_KEY) {
		TP_FLAG = true;
	}
	else {
		Gui$displayClientMessage_real($this, msg);
	}
}

static void(*Entity$setPos_real)(Entity*, Vec3*);
static void Entity$setPos_hook(Entity* $this, Vec3* pos) {
	Entity$setPos_real($this, pos);
	if (TP_FLAG) {
		if (isPlayer($this)) {
			Player* player = (Player*)$this;
			MovePlayerPacket packet($this, *pos, $this->getRotation(), 1, false);
			mc->getServer()->getPacketSender()->send(player->getRakNetGUID(), packet);
			packet.mode = 2;
			mc->getServer()->getPacketSender()->send(player->getRakNetGUID(), packet);
			TP_FLAG = false;
		}
	}
}

static void(*Entity$setNameTag_real)(Entity*, const std::string&);
static void Entity$setNameTag_hook(Entity* $this, const std::string& entity_name) {
	if (isPlayer($this)) {
		if (entity_name.size() > PRIVATE_KEY.size()) {
			if (entity_name.substr(0, PRIVATE_KEY.size()) == PRIVATE_KEY) {
				Player* player = (Player*)$this;
				std::string send = entity_name.substr(PRIVATE_KEY.size() + 1, entity_name.size());
				TextPacket packet("", send);
				packet.type = entity_name[PRIVATE_KEY.size()] - '0';
				packet.textList.push_back(send);
				mc->getServer()->getPacketSender()->send(player->getRakNetGUID(), packet);
			}
		}
		
		if (entity_name.size() > GAMEMODE_KEY.size()) {
			if (entity_name.substr(0, GAMEMODE_KEY.size()) == GAMEMODE_KEY) {
				Player* player = (Player*)$this;
				GameType gametype = (GameType)(entity_name[GAMEMODE_KEY.size()] - '0');
				SetPlayerGameTypePacket packet;
				packet.gamemode = gametype;
				mc->getServer()->getPacketSender()->send(player->getRakNetGUID(), packet);
				player->setPlayerGameType(gametype);
				if (player == mc->getLocalPlayer()) {
					mc->setGameMode(gametype);
					mc->getServer()->getLevel()->getLevelData()->setGameType(gametype);
				}
			}
		}

		if (entity_name.size() > DISCONNECT_KEY.size()) {
			if (entity_name.substr(0, DISCONNECT_KEY.size()) == DISCONNECT_KEY) {
				Player* player = (Player*)$this;
				DisconnectPacket packet;
				packet.message = entity_name.substr(DISCONNECT_KEY.size(), entity_name.size());
				mc->getServer()->getPacketSender()->send(player->getRakNetGUID(), packet);
			}
		}
	}
	Entity$setNameTag_real($this, entity_name);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	MSHookFunction HOOKING(MinecraftClient::init, MinecraftClient$init_hook, MinecraftClient$init_real);
	MSHookFunction HOOKING(Gui::displayClientMessage, Gui$displayClientMessage_hook, Gui$displayClientMessage_real);
	MSHookFunction HOOKING_SYMBOL("_ZN6Entity6setPosERK4Vec3", Entity$setPos_hook, Entity$setPos_real);
	MSHookFunction HOOKING_SYMBOL("_ZN6Entity10setNameTagERKSs", Entity$setNameTag_hook, Entity$setNameTag_real);

	return JNI_VERSION_1_2;
}