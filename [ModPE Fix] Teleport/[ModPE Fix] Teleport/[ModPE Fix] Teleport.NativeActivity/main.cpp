#include <jni.h>

#include "CustomFunctions.h"
#include "minecraftpe/client/MinecraftClient.h"
#include "minecraftpe/entity/player/Player.h"
#include "minecraftpe/network/protocol/MovePlayerPacket.h"
#include "hook/hook.h"
#include "access.h"

static MinecraftClient* mc = 0;
static void** ENTITY_VTABLE = (void**)dlsym(RTLD_DEFAULT, "_ZTV6Entity");

static const std::string KEY = "TELEPORT";
static bool TP_FLAG = false;

static bool operator==(Vec3& vec1, Vec3& vec2) {
	return vec1.x == vec2.x && vec1.y == vec2.y && vec1.z == vec2.z;
}

static void(*MinecraftClient$init_real)(MinecraftClient*);
static void MinecraftClient$init_hook(MinecraftClient* $this) {
	MinecraftClient$init_real($this);
	mc = $this;
}

static void(*Gui$displayClientMessage_real)(Gui*, const std::string&);
static void Gui$displayClientMessage_hook(Gui* $this, const std::string& msg) {
	if (msg == KEY) {
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
		void** vtable = access(void**, $this, 0);
		if (vtable[0x14C] != ENTITY_VTABLE[0x154]) {
			Player* player = (Player*)$this;
			MovePlayerPacket packet($this, *pos, $this->getRotation(), 1, false);
			mc->getServer()->getPacketSender()->send(player->getRakNetGUID(), packet);
			packet.mode = 2;
			mc->getServer()->getPacketSender()->send(player->getRakNetGUID(), packet);
			TP_FLAG = false;
		}
	}
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	MSHookFunction HOOKING(MinecraftClient::init, MinecraftClient$init_hook, MinecraftClient$init_real);
	MSHookFunction HOOKING(Gui::displayClientMessage, Gui$displayClientMessage_hook, Gui$displayClientMessage_real);
	MSHookFunction HOOKING_SYMBOL("_ZN6Entity6setPosERK4Vec3", Entity$setPos_hook, Entity$setPos_real);

	return JNI_VERSION_1_2;
}