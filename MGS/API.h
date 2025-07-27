#pragma once

#define CURL_STATICLIB
#include "curl/curl.h"
#include "SDK/SDK.hpp"
#include "CustomLogs.h"
#pragma comment(lib, "curl/libcurl.lib")
#pragma comment(lib, "curl/zlib.lib")
#include <map>
#include <iostream>
#include <thread>
using namespace std;

static bool bMcp = true;
static bool bUsingApi = true;
static bool bUsingWebhook = true;


static bool bDontNotifyMMS = false; // if true, the MMS will not be notified of the server going online!

// IF THIS IS TRUE IT WON'T NOTIFY MMS ANYWAY
static bool bStaffGameOnly = false; // if true, it will lock the game to only whitelisted usernames (and ips) from AdminNames and AdminIps

/*#ifdef EU
static string Region = "EU";
#else
static string Region = "NA";
#endif
#ifdef PLAYGROUND
static string RegionName = "NA Playground";
#else
#ifdef EU
static string RegionName = "EU Solo";
#else
#ifdef VPS
static string RegionName = "NA Solo";
#else
static string RegionName = "Testing server";
#endif
#endif
#endif
#ifdef VPS
#ifdef PLAYGROUND
static string ServerName = "PlooshFNSolosPG"; // EDIT THIS
#else
#ifdef EU
static string ServerName = "PlooshFNSolosEU"; // EDIT THIS
#else
static string ServerName = "PlooshFNSolos"; // EDIT THIS
#endif
#endif
#else
static string ServerName = "PlooshFNSolosLocal"; // EDIT THIS
#endif*/
#ifdef EU
static string Region = "EU";
#else
static string Region = "NAE";
#endif
#ifdef VPS
#ifdef ONE_SHOT
static string RegionName = "NA One Shot";
static string ServerName = "PlooshFNOneShot";
#elif defined(EU)
static string RegionName = "EU Solo";
static string ServerName = "PlooshFNSolosEU";
#else
static string RegionName = "NA Solo";
static string ServerName = "PlooshFNSolos";
#endif
#elif defined(MineekPrivate)
static string RegionName = "Mineek's Private Development Server";
static string ServerName = "MineekPrivate";
#elif defined(MineekPublic)
static string RegionName = "Mineek's Public Development Server";
static string ServerName = "MineekPublic";
#else
static string RegionName = "Eu";
static string ServerName = "PlooshFNSolosLocal";
#endif

static int PlayersOnBus = 0;

std::map<string, int> VbucksToGive{};

#ifdef EU
static string BACKEND_IP = "127.0.0.1";
static string VPS_IP = "20.13.160.153";
#define API_URL "https://backend.ploosh.dev:2053/"
#define MMS_URL "https://backend.ploosh.dev:2053/"
#elif defined(VPS)
/*static string BACKEND_IP = "vps.fn.ploosh.dev";
#ifndef PLAYGROUND
#ifdef EU
static string VPS_IP = "172.232.57.57";
#else
//static string VPS_IP = "173.255.238.183";
static string VPS_IP = "74.199.99.167";
#endif
#else
static string VPS_IP = "135.148.72.97";
#endif
#define API_URL "http://vps.fn.ploosh.dev:3551/"
#define MMS_URL "http://vps.fn.ploosh.dev:665/"*/
static string BACKEND_IP = "127.0.0.1";
static string VPS_IP = "74.199.99.167";
#define API_URL "https://backend.ploosh.dev/"
#define MMS_URL "https://backend.ploosh.dev/"
#else
static string BACKEND_IP = "127.0.0.1";
static string VPS_IP = "127.0.0.1";
#define API_URL "http://127.0.0.1:3551/"
#define MMS_URL "http://127.0.0.1:344/"
#endif
//#define WEBHOOK_URL "https://discordapp.com/api/webhooks/1177460796607971439/At4kPS850SSzJGtP3S0tDRTJGJ9zzRD3wea8u8GaODdvn-v5-VHKaQCgBcshNPfXd5UI"
//#define WEBHOOK_URL "https://discord.com/api/webhooks/1163243412863062016/KVKrMFdKnIheUMEavTQU1cekJrQjctlMAMWGr5N3EggNOIOaZmBiqFHUUiWQT57fXanW"
//#define WEBHOOK_URL "https://discord.com/api/webhooks/1198662622631960656/bnPrFdxJ7hnceEpDW_th_eo5GtIGo7Uswg-EvC-AhuBIRev9ldn11dgooKVgDGTDsa4O"
#define WEBHOOK_URL "https://discord.com/api/webhooks/1364219211169271888/cZfgogL2QbE3Tgpu6iOlNP7HsDX3LDz3SpTv898alk34Rw_f7cpXX3HWNrShQGp6tSbK"

static size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* RES)
{
	if (!contents || !RES)
		return 0;

	((std::string*)RES)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static size_t LogData(char* contents, size_t size, size_t nmemb, void* RES)
{
	if (!contents || !RES)
		return 0;

	//((std::string*)RES)->append((char*)contents, size * nmemb);
	log_debug("Response: %s\n", contents);
	return size * nmemb;
}

static string ReplaceString(string str, string substr1, std::string substr2)
{
	for (size_t index = str.find(substr1, 0); index != std::string::npos && substr1.length(); index = str.find(substr1, index + substr2.length()))
		str.replace(index, substr1.length(), substr2);
	return str;
}


class DefaultAPI
{
public:
	DefaultAPI()
	{
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();

		if (!curl)
		{
			return;
		}

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));
	}

	~DefaultAPI() {
		curl_global_cleanup();
		curl_easy_cleanup(curl);
	}

	FORCEINLINE bool PerformAction(const std::string& Endpoint, std::string* OutResponse = nullptr)
	{
		try
		{
			std::string URL = API_URL + Endpoint;


			auto out1 = curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

			if (out1 != CURLE_OK)
			{
				log_error("Curl setopt failed!\n");
				return false;
			}

			std::string TemporaryBuffer;
			if (OutResponse)
			{
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &TemporaryBuffer);
			}
			else {
				curl_easy_reset(curl);
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));
				auto out1 = curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, LogData);

				if (out1 != CURLE_OK)
				{
					log_error("Curl setopt failed!\n");
					return false;
				}
			}

			auto out2 = curl_easy_perform(curl);
			//log_debug("%s\n", out2);

			if (out2 != CURLE_OK)
			{
				log_error("Request failed!\n");
				return false;
			}

			if (OutResponse != nullptr) *OutResponse = TemporaryBuffer;
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	FORCEINLINE bool PerformActionMMS(const std::string& Endpoint, std::string* OutResponse = nullptr)
	{
		try
		{
			std::string URL = MMS_URL + Endpoint;

			auto out1 = curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

			if (out1 != CURLE_OK)
			{
				log_error("Curl setopt failed!\n");
				return false;
			}

			std::string TemporaryBuffer;
			if (OutResponse)
			{
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &TemporaryBuffer);
			}
			else {
				curl_easy_reset(curl);
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));
				auto out1 = curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

				if (out1 != CURLE_OK)
				{
					log_error("Curl setopt failed!\n");
					return false;
				}
			}

			auto out2 = curl_easy_perform(curl);
			//log_debug("%s\n", out2);
			if (out2 != CURLE_OK)
			{
				log_error("Request failed!\n");
				return false;
			}
		}
		catch (...)
		{
			return false;
		}

		return true;
	}
protected:
	CURL* curl;
};

static size_t write_callback(char* ptr, size_t size, size_t nmenb, void* userdata) {
	((std::string*)userdata)->append(ptr, size * nmenb);
	return size * nmenb;
}

std::string iso8601() {
	time_t now;
	time(&now);
	char buf[sizeof "2011-10-08T07:07:09Z"];
	tm* t = new tm();
	gmtime_s(t, &now);
	strftime(buf, sizeof buf, "%FT%TZ", t);
	return string(buf);
}

namespace PlooshMMSAPI {
	inline bool MarkServerOnlinev2(string REGION, string PlayerCap, string Port, string Session, string Playlist, string CustomCode) {

		std::string Endpoint = std::format("plooshfn/gs/create/session/{}/{}/{}/{}/{}", REGION, VPS_IP, Port, Playlist, ServerName);

		std::string fullEndpoint = MMS_URL + Endpoint;

		curl_global_init(CURL_GLOBAL_ALL);
		CURL* curl = curl_easy_init();
		if (!curl) {
			log_error("Failed to initialize libcurl.\n");
			curl_global_cleanup();
		}

		//Set URL to API endpoint
		curl_easy_setopt(curl, CURLOPT_URL, fullEndpoint.c_str());


		// Set callback function for response body
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		// Create a buffer to store the response body
		std::string response_body;

		// Set the buffer as the user-defined data for the callback function
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

		// Perform HTTP request
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			//log_error("Failed to perform HTTP request: %s\n", curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			//UptimeWebHook.send_message("Failed to perform HTTP request for getting skin");
			return false;
		}

		// Check HTTP response code
		long response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		if (response_code >= 200 && response_code < 300) {
			// HTTP request successful, check response body
			curl_easy_cleanup(curl);
			curl_global_cleanup();

			//UptimeWebHook.send_message("HTTP request successful for getting skin" + response_body);
			return true;

		}
		else {
			// HTTP request failed
			//log_error("HTTP request failed with status code %ld.\n", response_code);
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			//UptimeWebHook.send_message("HTTP request failed with status code " + std::to_string(response_code) + " for getting skin");
			return false;
		}

	}

	inline bool SetServerStatus(string status) {

		std::string Endpoint = std::format("plooshfn/gs/status/set/{}/{}", ServerName, status);

		std::string fullEndpoint = MMS_URL + Endpoint;

		curl_global_init(CURL_GLOBAL_ALL);
		CURL* curl = curl_easy_init();
		if (!curl) {
			log_error("Failed to initialize libcurl.\n");
			curl_global_cleanup();
		}

		//Set URL to API endpoint
		curl_easy_setopt(curl, CURLOPT_URL, fullEndpoint.c_str());


		// Set callback function for response body
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		// Create a buffer to store the response body
		std::string response_body;

		// Set the buffer as the user-defined data for the callback function
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

		// Perform HTTP request
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			//log_error("Failed to perform HTTP request: %s\n", curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			//UptimeWebHook.send_message("Failed to perform HTTP request for getting skin");
			return false;
		}

		// Check HTTP response code
		long response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		if (response_code >= 200 && response_code < 300) {
			// HTTP request successful, check response body
			curl_easy_cleanup(curl);
			curl_global_cleanup();

			//UptimeWebHook.send_message("HTTP request successful for getting skin" + response_body);
			return true;

		}
		else {
			// HTTP request failed
			//log_error("HTTP request failed with status code %ld.\n", response_code);
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			//UptimeWebHook.send_message("HTTP request failed with status code " + std::to_string(response_code) + " for getting skin");
			return false;
		}

	}


}

namespace Discord {
	class DiscordWebhook {
	public:
		DiscordWebhook(const char* webhook) {
			curl_global_init(CURL_GLOBAL_ALL);
			curl = curl_easy_init();

			if (curl) {
				curl_easy_setopt(curl, CURLOPT_URL, webhook);
				curl_slist* headers = curl_slist_append(NULL, "Content-Type: application/json");
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			}
			else {
				log_error("Failed to send! Will return as a null pointer.\n");
			}
		}

		~DiscordWebhook() {
			curl_global_cleanup();
			curl_easy_cleanup(curl);
		}

		inline bool send_message(const std::string& message) {
			if (!bUsingWebhook)
				return false;
			std::string json = "{\"content\": \"" + message + +"\"}";
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

			return curl_easy_perform(curl) == CURLE_OK;
		}

		inline bool send_status(const std::string& version, std::string& playlist, int color = 0)
		{
			std::string json = "{\"embeds\": [{\"title\": \"Server up!\", \"fields\": [{\"name\":\"Version\",\"value\":\"" + version + "\"},{\"name\":\"Region\",\"value\":\"" + Region + "\"},{\"name\":\"Playlist\",\"value\":\"" + playlist + "\"}], \"color\": " + "\"" + std::to_string(color) + "\", \"footer\": {\"text\":\"Astro\", \"icon_url\":\"https://media.discordapp.net/attachments/1190820081639358464/1193388772142878730/ac54ee68ce1ab734b435abec0b5f9a60.png\"}, \"timestamp\":\"" + iso8601() + "\"}] }";
			// std::cout << "json: " << json << '\n';
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

			bool success = curl_easy_perform(curl) == CURLE_OK;

			return success;
		}

		inline bool send_started(const std::string& version, std::string& players, int color = 0)
		{
			std::string json = "{\"embeds\": [{\"title\": \"Match started!\", \"fields\": [{\"name\":\"Version\",\"value\":\"" + version + "\"},{\"name\":\"Region\",\"value\":\"" + Region + "\"},{\"name\":\"Players\",\"value\":\"" + players + "\"}], \"color\": " + "\"" + std::to_string(color) + "\", \"footer\": {\"text\":\"Astro\", \"icon_url\":\"https://media.discordapp.net/attachments/1190820081639358464/1193388772142878730/ac54ee68ce1ab734b435abec0b5f9a60.png\"}, \"timestamp\":\"" + iso8601() + "\"}] }";
			// std::cout << "json: " << json << '\n';
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

			bool success = curl_easy_perform(curl) == CURLE_OK;

			return success;
		}

		inline bool send_restarting(const std::string& version, int color = 0)
		{
			std::string json = "{\"embeds\": [{\"title\": \"Server restarting...\", \"fields\": [{\"name\":\"Version\",\"value\":\"" + version + "\"},{\"name\":\"Region\",\"value\":\"" + Region + "\"}], \"color\": " + "\"" + std::to_string(color) + "\", \"footer\": {\"text\":\"Astro\", \"icon_url\":\"https://media.discordapp.net/attachments/1190820081639358464/1193388772142878730/ac54ee68ce1ab734b435abec0b5f9a60.png\"}, \"timestamp\":\"" + iso8601() + "\"}] }";
			// std::cout << "json: " << json << '\n';
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

			bool success = curl_easy_perform(curl) == CURLE_OK;

			return success;
		}
	private:
		CURL* curl;
	};


	static DiscordWebhook Servers(WEBHOOK_URL);

}

namespace PlooshFNAPI
{
	static DefaultAPI* StaticAPI = nullptr;
	void SetupAPI()
	{
		if (!bUsingApi)
			return;

		if (!StaticAPI) StaticAPI = new DefaultAPI();
	}

	bool CheckUser(string AccountID)
	{
		if (StaticAPI)
		{
			std::string Endpoint = std::format("plooshfn/check/{}", AccountID);
			string OutResponse = "false";

			if (!StaticAPI->PerformAction(Endpoint, &OutResponse))
			{
				log_warn("Request failed\n");
			}

			return OutResponse == "true";
		}
		return false;
	}

	bool CheckReboot(string AccountID)
	{
		if (StaticAPI)
		{
			std::string Endpoint = std::format("plooshfn/reboot/{}", AccountID);
			string OutResponse = "false";

			if (!StaticAPI->PerformAction(Endpoint, &OutResponse))
			{
				log_warn("Request failed\n");
			}

			return OutResponse == "true";
		}
		return false;
	}

	void MarkServerOnline(string REGION, string PlayerCap, string Port, string Session, string Playlist, string CustomCode)
	{
		//Discord::Servers.send_message(RegionName + " is now up!");
		Discord::Servers.send_status("12.41", Playlist, 0x2b2d31);
		//if (StaticAPI)
		if (false)
		{
			std::string Endpoint = std::format("plooshfn/gs/add/{}/{}/{}/{}/{}/{}/{}", Session, REGION, VPS_IP, Port, CustomCode, Playlist, PlayerCap);
			if (!StaticAPI->PerformAction(Endpoint))
			{
				log_warn("Request failed\n");
			}
		}
	}

	void LockGameSession(string Region, string Name, int Players)
	{
		//Discord::Servers.send_message("Match started on " + RegionName + " with " + std::to_string(Players) + (Players == 1 ? " player!" : " players!"));
		Discord::Servers.send_started("12.41", (std::string&)to_string(Players), 0x2b2d31);
		//if (StaticAPI)
		if (false)
		{
			std::string Enp = std::format("plooshfn/gs/lock/{}/{}", Region, Name);
			if (!StaticAPI->PerformAction(Enp))
			{
				log_warn("Request failed\n");
			}
		}
	}

	void RemoveSession(string Region, string Name)
	{
		//Discord::Servers.send_message(RegionName + " is restarting...");
		Discord::Servers.send_restarting("12.41", 0x2b2d31);
		//if (StaticAPI)
		if (false)
		{
			std::string Endp = std::format("plooshfn/gs/player/remove/{}/{}", Name, Region);
			if (!StaticAPI->PerformAction(Endp))
			{
				log_warn("Request failed\n");
			}
		}
	}

	void AddVbucksOK(string Amount, string AccountID/*, string xp*/)
	{
		if (StaticAPI)
		{
			log_debug("%s\n", AccountID.c_str());
//#ifdef VPS
		//	std::string Endp = std::format("http://127.0.0.1:3551/api/reload/vbucks?apikey", AccountID, Amount/*, xp*/);
//#else
			std::string Endp = std::format("http://127.0.0.1:3551/api/reload/vbucks?apikey", AccountID, Amount);
//#endif
			string ret = "idajasdkjfa";
			if (!StaticAPI->PerformAction(Endp, &ret))
			{
				log_warn("Request failed\n");
			}
			log_debug("%s\n", ret.c_str());
		}
	}

	void GiveVbucks(string Amount, string AccountID/*, string xp*/)
	{
		//if (PlayersOnBus > 1)
		//	AddVbucksOK(Amount, PlaylistName, WinOrNot, AccountID);
		if (PlayersOnBus > 1) new thread(AddVbucksOK, Amount, AccountID/*, xp*/);
	}

	void BanPlayer(string AccountID)
	{
		if (StaticAPI)
		{
			std::string Endp = std::format("plooshfn/ban/{}", AccountID);
			if (!StaticAPI->PerformAction(Endp))
			{
				log_warn("Request failed\n");
			}
		}
	}

	bool IsBanned(string AccountID)
	{
		//return false;
		if (!StaticAPI)
			return false;

		std::string Endp = std::format("plooshfn/ban/check/{}", AccountID);
		string OutResponse = "false";
		if (!StaticAPI->PerformAction(Endp, &OutResponse))
		{
			log_warn("Request failed\n");
			return false;
		}

		return OutResponse == "true";
	}

	void AddXP(string AccountID, int xp)
	{
		//return false;
		if (!StaticAPI)
			return;

		std::string Endp = std::format("plooshfn/xp/{}/{}", AccountID, std::to_string(xp));
		if (!StaticAPI->PerformAction(Endp))
		{
			log_warn("Request failed\n");
			return;
		}

		return;
	}
}

//hosting@gmail.com
//3d7972759df74270bac6b09f992dd7b1