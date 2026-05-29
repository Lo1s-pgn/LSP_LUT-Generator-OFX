#pragma once
/* Session header on plug-in construct; errors append with mutex. */
#include <chrono>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#if defined(__APPLE__)
#include <dlfcn.h>
#include <limits.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#elif defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif defined(__unix__)
#include <limits.h>
#include <sys/utsname.h>
#endif

namespace LSPLutGeneratorLog {

inline std::mutex& getLogMutex() {
    static std::mutex s_mutex;
    return s_mutex;
}

inline const char* getHomeEnv() {
#if defined(_WIN32)
    const char* home = std::getenv("USERPROFILE");
    if (!home || home[0] == '\0')
        home = std::getenv("HOME");
#else
    const char* home = std::getenv("HOME");
#endif
    return home;
}

inline std::string getLogPath() {
#if defined(_WIN32)
    const char* appData = std::getenv("APPDATA");
    if (!appData || appData[0] == '\0')
        return std::string("LutGenerator.log");
    return std::string(appData) + "\\LSP\\LutGenerator.log";
#else
    const char* home = getHomeEnv();
    if (!home || home[0] == '\0')
        return std::string("/tmp/LutGenerator.log");
    return std::string(home) + "/Library/Application Support/LSP/LutGenerator.log";
#endif
}

inline std::string sanitizePathForLog(const std::string& p) {
    const char* home = getHomeEnv();
    if (!home || home[0] == '\0' || p.empty())
        return p;
    std::string hp(home);
#if defined(_WIN32)
    if (p.size() >= hp.size() && _stricmp(p.substr(0, hp.size()).c_str(), hp.c_str()) == 0
        && (p.size() == hp.size() || p[hp.size()] == '\\' || p[hp.size()] == '/'))
        return std::string("~") + p.substr(hp.size());
#else
    if (p.size() >= hp.size() && p.compare(0, hp.size(), hp) == 0
        && (p.size() == hp.size() || p[hp.size()] == '/'))
        return std::string("~") + p.substr(hp.size());
#endif
    return p;
}

inline bool ensureLogDirectoryExists(const std::string& logPath) {
    namespace fs = std::filesystem;
    std::error_code ec;
    const fs::path parent = fs::path(logPath).parent_path();
    if (parent.empty())
        return true;
    fs::create_directories(parent, ec);
    return !ec;
}

inline std::string getTimestamp(const char* fmt = "%Y-%m-%d %H:%M:%S") {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    char buf[64];
    struct tm tm_buf;
#if defined(_WIN32)
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    std::strftime(buf, sizeof(buf), fmt, &tm_buf);
    return std::string(buf);
}

inline bool openLogFile(std::ofstream& f, std::ios_base::openmode mode = std::ios::app) {
    std::string path = getLogPath();
    if (!ensureLogDirectoryExists(path))
        return false;
    f.open(path, mode);
    return f.good();
}

inline int getCoreCount() {
#if defined(__APPLE__)
    int ncpu = 0;
    size_t len = sizeof(ncpu);
    if (sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0) == 0)
        return ncpu;
#elif defined(_WIN32)
    SYSTEM_INFO info{};
    GetSystemInfo(&info);
    if (info.dwNumberOfProcessors > 0)
        return static_cast<int>(info.dwNumberOfProcessors);
#endif
    return 0;
}

#if defined(__APPLE__) || defined(__unix__)
inline void getUnameFields(std::string& sysname, std::string& nodename, std::string& release, std::string& version, std::string& machine) {
    struct utsname u;
    sysname = nodename = release = version = machine = "unknown";
    if (uname(&u) == 0) {
        sysname = u.sysname;
        nodename = u.nodename;
        release = u.release;
        version = u.version;
        machine = u.machine;
    }
}
#endif

inline std::string getCpuInfo() {
#if defined(__APPLE__)
    char buf[256];
    size_t len = sizeof(buf);
    if (sysctlbyname("machdep.cpu.brand_string", buf, &len, NULL, 0) == 0 && len > 0)
        return std::string(buf, len - 1);
    len = sizeof(buf);
    if (sysctlbyname("hw.model", buf, &len, NULL, 0) == 0 && len > 0)
        return std::string(buf, len - 1);
#elif defined(_WIN32)
    char buf[256] = "Windows";
    return std::string(buf);
#endif
    return "unknown";
}

inline std::string getMemoryInfoMB() {
#if defined(__APPLE__)
    uint64_t memsize = 0;
    size_t len = sizeof(memsize);
    if (sysctlbyname("hw.memsize", &memsize, &len, NULL, 0) == 0)
        return std::to_string(memsize / (1024 * 1024)) + " MB";
#elif defined(_WIN32)
    MEMORYSTATUSEX st{};
    st.dwLength = sizeof(st);
    if (GlobalMemoryStatusEx(&st))
        return std::to_string(st.ullTotalPhys / (1024 * 1024)) + " MB";
#endif
    return "unknown";
}

#if defined(__APPLE__)
static void lutGenLogBundleAnchor() {}

inline std::string getPluginBundleRootPath() {
    Dl_info info{};
    if (dladdr(reinterpret_cast<void*>(&lutGenLogBundleAnchor), &info) == 0 || !info.dli_fname)
        return "";
    std::string p = info.dli_fname;
    char resolved[PATH_MAX];
    if (realpath(p.c_str(), resolved))
        p = resolved;
    const char* marker = ".ofx.bundle";
    size_t pos = p.find(marker);
    if (pos == std::string::npos)
        return "";
    return p.substr(0, pos + std::strlen(marker));
}
#elif defined(_WIN32)
inline std::string getPluginBundleRootPath() {
    HMODULE mod = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&getPluginBundleRootPath), &mod)
        || !mod)
        return "";
    wchar_t wpath[MAX_PATH];
    const DWORD n = GetModuleFileNameW(mod, wpath, MAX_PATH);
    if (n == 0 || n >= MAX_PATH)
        return "";
    int needed = WideCharToMultiByte(CP_UTF8, 0, wpath, -1, nullptr, 0, nullptr, nullptr);
    if (needed <= 1)
        return "";
    std::string p(static_cast<size_t>(needed - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wpath, -1, p.data(), needed, nullptr, nullptr);
    const char* marker = ".ofx.bundle";
    size_t pos = p.find(marker);
    if (pos == std::string::npos)
        return "";
    return p.substr(0, pos + std::strlen(marker));
}
#else
inline std::string getPluginBundleRootPath() {
    return "";
}
#endif

inline void writeSessionStart(const std::string& pluginName,
    const std::string& versionStr,
    const std::string& hostName,
    const std::string& hostLabel,
    const std::string& hostVersion,
    const std::string& buildInfo,
    const std::string& bundlePath,
    const std::string& gpuName) {
    std::lock_guard<std::mutex> lock(getLogMutex());
    std::ofstream f;
    if (!openLogFile(f, std::ios::out))
        return;
    const std::string bufTime = getTimestamp("%a %b %d %H:%M:%S %Y");

    f << "------------------------------------------------------------\n";
    f << "\t\t" << pluginName << "\n";
    f << "------------------------------------------------------------\n";
    f << "> Plugin Version\t: " << versionStr << "\n";
    f << "> Timestamp\t\t: " << bufTime << "\n";
    f << "> Host System Info: \n";

#if defined(__APPLE__) || defined(__unix__)
    std::string sysname, nodename, release, version, machine;
    getUnameFields(sysname, nodename, release, version, machine);
    (void)nodename;
    f << "\t- OS: \n";
    f << "\t\t" << sysname << "\n";
    f << "\t\t" << release << "\n";
    f << "\t\t" << version << "\n";
    f << "\t\t" << machine << "\n";
#elif defined(_WIN32)
    f << "\t- OS: \n\t\tWindows\n";
#endif
    f << "\t- CPU: \n\t\t" << getCpuInfo() << "\n";
    int cores = getCoreCount();
    if (cores > 0)
        f << "\t- Cores: \n\t\t" << cores << "\n";
    f << "\t- Memory: \n\t\t" << getMemoryInfoMB() << "\n";
    if (!gpuName.empty())
        f << "\t- GPU: \n\t\t" << gpuName << "\n";
    std::string hostDisplay = hostLabel.empty() ? hostName : hostLabel;
    if (hostDisplay.empty())
        hostDisplay = hostName.empty() ? "unknown" : hostName;
    f << "\t- Host: \n\t\t" << hostDisplay << "\n";
    if (!hostVersion.empty())
        f << "\t- Host version: \n\t\t" << hostVersion << "\n";
    if (!buildInfo.empty())
        f << "\t- Build: \n\t\t" << buildInfo << "\n";
    f << "\t- Renderer: \n\t\tCPU\n";

    f << "\n------------------------------------------------------------\n";
    f << "[info] Logging start\n";
    if (!bundlePath.empty())
        f << "[info] Bundle path: " << sanitizePathForLog(bundlePath) << "/\n";
    f.flush();
}

inline void writeErrorLine(const std::string& message) {
    std::lock_guard<std::mutex> lock(getLogMutex());
    std::ofstream f;
    if (!openLogFile(f))
        return;
    f << getTimestamp() << " [error] " << message << "\n";
    f.flush();
}

inline void writeInfoLine(const std::string& message) {
    std::lock_guard<std::mutex> lock(getLogMutex());
    std::ofstream f;
    if (!openLogFile(f))
        return;
    f << getTimestamp() << " [info] " << message << "\n";
    f.flush();
}

} // namespace LSPLutGeneratorLog

#define LSP_LUTGEN_LOG_ERROR(msg) LSPLutGeneratorLog::writeErrorLine(std::string(msg))
#define LSP_LUTGEN_LOG_INFO(msg) LSPLutGeneratorLog::writeInfoLine(std::string(msg))
#define LSP_LUTGEN_LOG_SESSION_START(n, v, hn, hl, hver, build, bundle, gpu) \
    LSPLutGeneratorLog::writeSessionStart(std::string(n), std::string(v), std::string(hn), std::string(hl), std::string(hver), std::string(build), std::string(bundle), std::string(gpu))
