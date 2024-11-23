#include "Hooks.h"
#include "Manager.h"

namespace Hooks::Detach
{
	// clears light from the shadowscenenode
	struct RemoveLight
	{
		static void thunk(RE::TESObjectREFR* a_this, bool a_isMagicLight)
		{
			LightManager::GetSingleton()->DetachLights(a_this, false);

			func(a_this, a_isMagicLight);
		};
		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(19253, 19679) };
			stl::hook_function_prologue<RemoveLight, 8>(target.address());

			logger::info("Hooked TESObjectREFR::RemoveLight");
		}
	};

	// clears light from the shadowscenenode + nilight ptr
	struct GetLightData
	{
		static RE::REFR_LIGHT* thunk(RE::ExtraDataList* a_list)
		{
			if (auto* ref = stl::adjust_pointer<RE::TESObjectREFR>(a_list, -0x70)) {
				LightManager::GetSingleton()->DetachLights(ref, true);
			}

			return func(a_list);
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			std::array targets{
				std::make_pair(RELOCATION_ID(19102, 19504), OFFSET(0xC0, 0xCA)),    // TESObjectREFR::ClearData
				std::make_pair(RELOCATION_ID(19302, 19729), OFFSET(0x63C, 0x63A)),  // TESObjectREFR::Set3D
			};

			for (auto& [address, offset] : targets) {
				REL::Relocation<std::uintptr_t> target{ address, offset };
				stl::write_thunk_call<GetLightData>(target.address());
			}

			logger::info("Hooked ExtraDataList::GetLightData");
		}
	};

	struct RunBiped3DDetach
	{
		static void thunk(const RE::ActorHandle& a_handle, RE::NiAVObject* a_node)
		{
			LightManager::GetSingleton()->DetachWornLights(a_handle, a_node);

			func(a_handle, a_node);
		};
		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(15495, 15660) };
			stl::hook_function_prologue<RunBiped3DDetach, 5>(target.address());

			logger::info("Hooked BipedAnim::RunBiped3DDetach");
		}
	};

	void Install()
	{
		RemoveLight::Install();
		GetLightData::Install();
		RunBiped3DDetach::Install();
	}
}
