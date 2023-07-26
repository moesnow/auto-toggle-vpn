#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace std::filesystem;
using json = nlohmann::json;

// Function declarations
void generateExampleConfigFile(const path& configFilePath);
void printHelp();
void printVersion();
void printUnknown(const string& arg);
vector<string> executeCommand(const string& command);
vector<string> getAllEthernetInterfaces();
vector<string> getAllWiFiInterfaces();
bool isInterfaceActive(const string& interfaceName);
bool isAllInterfaceActive(const vector<string>& interfaceNames);
string getCurrentWiFiSSID();
bool isVPNActive(const string& name);
void toggleVPN(const string& serviceName, bool connect);
bool isSSIDHit(const string& currentSSID, const vector<string>& SSIDList);

// Constants
const string ProgramName = "auto-toggle-vpn";
const string ProgramVersion = "0.0.3";
const string configFileName = "config.json";
const path configFilePath = string(getenv("HOME")) + "/.local/" + ProgramName + "/" + configFileName;

// Struct to hold configuration values
struct Config {
    string vpnname;
    vector<string> ssidlist;
};

// Main function
int main(int argc, char* argv[]) {
    if (argc > 1) {
        string arg(argv[1]);
        if (arg == "-g" or arg == "--generate") {
            generateExampleConfigFile(configFilePath.string() + ".example");
        } else if (arg == "-v" or arg == "--version") {
            printVersion();
        } else if (arg == "-h" or arg == "--help") {
            printHelp();
        } else {
            printUnknown(arg);
            return 1;
        }
        return 0;
    }

    ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        cerr << "Error: Failed to open config file." << endl;
        return 1;
    }

    Config config;
    try {
        json configJson;
        configFile >> configJson;
        config.vpnname = configJson["vpnname"].get<string>();
        config.ssidlist = configJson["ssidlist"].get<vector<string>>();
    } catch (const exception& e) {
        cerr << "Error: Failed to parse config file. " << e.what() << endl;
        return 1;
    }

    bool isEthernetActive = isAllInterfaceActive(getAllEthernetInterfaces());
    bool isWiFiActive = isAllInterfaceActive(getAllWiFiInterfaces());
    bool isVPNConnected = isVPNActive(config.vpnname);

    if (!isEthernetActive and !isWiFiActive) {
        if (isVPNConnected) {
            cout << "Stopping VPN" << endl;
            toggleVPN(config.vpnname, false);
        }
    } else {
        string currentSSID = getCurrentWiFiSSID();
        if (isEthernetActive or isSSIDHit(currentSSID, config.ssidlist)) {
            if (isVPNConnected) {
                cout << "Stopping VPN" << endl;
                toggleVPN(config.vpnname, false);
            }
        } else {
            if (!isVPNConnected) {
                cout << "Starting VPN" << endl;
                toggleVPN(config.vpnname, true);
            }
        }
    }

    return 0;
}

// Utility functions
void generateExampleConfigFile(const path& configFilePath) {
    Config exampleConfig;
    exampleConfig.vpnname = "Enter your vpn_name, You can find it in Settings - VPN.";
    exampleConfig.ssidlist = {"SSID_1", "SSID_2"};

    try {
        create_directories(configFilePath.parent_path());

        ofstream configFile(configFilePath);
        if (!configFile.is_open()) {
            cerr << "Error: Failed to create example configuration file." << endl;
            return;
        }

        json configJson;
        configJson["vpnname"] = exampleConfig.vpnname;
        configJson["ssidlist"] = exampleConfig.ssidlist;

        configFile << configJson.dump(4) << endl;
        configFile.close();

        cout << "Successfully generated an example configuration file.\n"
             << "Please make changes to the file located at " << configFilePath
             << " and rename it to 'config.json'." << endl;
    } catch (const exception& e) {
        cerr << "Error: Failed to generate example configuration file. " << e.what() << endl;
    }
}

void printHelp() {
    cout << "Usage: " << ProgramName << " [options]\n"
         << "Options:\n"
         << "  -g, --generate   : Generate config file example\n"
         << "  -v, --version    : Display program version\n"
         << "  -h, --help       : Display this help message\n";
}

void printVersion() {
    cout << "Program version: " << ProgramVersion << endl;
}

void printUnknown(const string& arg) {
    cout << ProgramName + ": option " << arg << " is unknown.\n"
         << ProgramName + ": try '" + ProgramName + " --help' for more information.\n";
}

vector<string> executeCommand(const string& command) {
    vector<string> result = {};
    array<char, 128> buffer;
    shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        cerr << "Error: Failed to execute command." << endl;
        return result;
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result.push_back(buffer.data());
    }
    for (auto& str : result) {
        if (!str.empty() && str.back() == '\n') {
            str.pop_back();  // Remove newline character
        }
    }
    if (result.empty()) {
        result.push_back("");
    }
    return result;
}

vector<string> getAllEthernetInterfaces() {
    string command = "networksetup -listnetworkserviceorder | sed -nE '/Ethernet|Dock|LAN/ s|.*Device: (en[0-9]+).*|\\1|p'";
    return executeCommand(command);
}

vector<string> getAllWiFiInterfaces() {
    string command = "networksetup -listnetworkserviceorder | sed -nE '/Wi-Fi|AirPort/ s|.*Device: (en[0-9]+).*|\\1|p'";
    return executeCommand(command);
}

bool isInterfaceActive(const string& interfaceName) {
    string command = "ifconfig " + interfaceName + " 2>/dev/null | awk '/status/ {print $2}'";
    return executeCommand(command).front() == "active";
}

bool isAllInterfaceActive(const vector<string>& interfaceNames) {
    for (const auto& interface : interfaceNames) {
        if (isInterfaceActive(interface)) {
            return true;
        }
    }
    return false;
}

string getCurrentWiFiSSID() {
    string command = "/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I | awk -F': ' '/^ *SSID:/ {print $2}'";
    return executeCommand(command).front();
}

bool isVPNActive(const string& name) {
    string command = "networksetup -showpppoestatus " + name;
    return executeCommand(command).front() == "connected";
}

void toggleVPN(const string& serviceName, bool connect) {
    string command;
    if (connect) {
        command = "networksetup -connectpppoeservice \"" + serviceName + "\"";
    } else {
        command = "networksetup -disconnectpppoeservice \"" + serviceName + "\"";
    }
    executeCommand(command);
}

bool isSSIDHit(const string& currentSSID, const vector<string>& SSIDList) {
    for (string ssid : SSIDList) {
        if (currentSSID == ssid) {
            return true;
        }
    }
    return false;
}
