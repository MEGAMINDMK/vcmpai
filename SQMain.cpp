#include "SQMain.h"
#include "SQConsts.h"
#include "SQFuncs.h"
#include "GroqAPI.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <algorithm>

// ================= VCMP =================
PluginFuncs* VCMP;
HSQUIRRELVM v;
HSQAPI sq;

// ================= CONFIG =================
struct AIConfig
{
    std::string apiKey;
    std::string model;
};

AIConfig Config;

// ================= LOG =================
void Log(const std::string& msg)
{
    printf("[AI] %s\n", msg.c_str());
}

// ================= LOAD CONFIG =================
void LoadConfig()
{
    FILE* f = fopen("ai.cfg", "r");

    if (!f)
    {
        f = fopen("ai.cfg", "w");

        fprintf(f,
            "api_key=YOUR_API_KEY_HERE\n"
            "model=meta-llama/llama-4-scout-17b-16e-instruct\n"
        );

        fclose(f);
        return;
    }

    char line[512];

    while (fgets(line, sizeof(line), f))
    {
        std::string s(line);

        s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
        s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());

        size_t pos = s.find('=');
        if (pos == std::string::npos) continue;

        std::string key = s.substr(0, pos);
        std::string value = s.substr(pos + 1);

        if (key == "api_key") Config.apiKey = value;
        else if (key == "model") Config.model = value;
    }

    fclose(f);
}

// ================= AI REQUEST =================
std::string AskAI(const std::string& msg, const std::string& tone)
{
    try
    {
        std::string prompt = "[" + tone + " tone] " + msg;

        std::string response =
            Groq::GroqApi::GetInstance().SendChatRequest(
                Config.apiKey,
                Config.model,
                prompt
            );

        json j = json::parse(response);

        if (j.contains("choices") && j["choices"].size() > 0)
            return j["choices"][0]["message"]["content"];

        return "AI error";
    }
    catch (const std::exception& e)
    {
        Log(e.what());
        return "AI failed";
    }
}

// ================= SQUIRREL FUNCTION =================
SQInteger fn_getChatGPTResponse(HSQUIRRELVM v)
{
    const char* text = nullptr;
    const char* tone = nullptr;
    SQInteger player = -1; // ✔ FIXED TYPE

    sq->getstring(v, 2, &text);
    sq->getinteger(v, 3, &player);
    sq->getstring(v, 4, &tone);

    std::string reply = AskAI(text ? text : "", tone ? tone : "friendly");

    sq->pushstring(v, reply.c_str(), -1);
    return 1;
}

// ================= SQHOST2 ATTACH =================
void OnSquirrelScriptLoad()
{
    size_t size;
    int32_t sqId = VCMP->FindPlugin(const_cast<char*>("SQHost2"));
    const void** sqExports = VCMP->GetPluginExports(sqId, &size);

    if (sqExports && size > 0)
    {
        SquirrelImports** sqDeref = (SquirrelImports**)sqExports;
        SquirrelImports* sqFuncs = (SquirrelImports*)(*sqDeref);

        if (sqFuncs)
        {
            v = *(sqFuncs->GetSquirrelVM());
            sq = *(sqFuncs->GetSquirrelAPI());

            RegisterFuncswebnet(v);

            Log("AI system loaded");
        }
    }
}

// ================= PLUGIN COMMAND =================
uint8_t OnPluginCommand(uint32_t type, const char* text)
{
    if (type == 0x7D6E22D8)
        OnSquirrelScriptLoad();

    return 1;
}

// ================= INIT =================
uint8_t OnServerInitialise()
{
    LoadConfig();
    OutputMessage("MEGAMIND AI Plugin Loaded");
    return 1;
}

// ================= PLAYER JOIN =================
void playerCreated(int32_t playerId)
{
    VCMP->SendClientMessage(playerId, 0xFFFFFFFF, "This server uses A.I plugin By MEGAMIND!");
}

// ================= ENTRY =================
extern "C" unsigned int VcmpPluginInit(
    PluginFuncs* pluginFuncs,
    PluginCallbacks* pluginCalls,
    PluginInfo* pluginInfo)
{
    VCMP = pluginFuncs;

    pluginInfo->pluginVersion = 0x110;
    pluginInfo->apiMajorVersion = PLUGIN_API_MAJOR;
    pluginInfo->apiMinorVersion = PLUGIN_API_MINOR;

    strcpy(pluginInfo->name, "Webnet-AI");

    pluginCalls->OnServerInitialise = OnServerInitialise;
    pluginCalls->OnPluginCommand = OnPluginCommand;
    pluginCalls->OnPlayerConnect = playerCreated;

    return 1;
}