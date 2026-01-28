#include "stdafx.h"
#include "CustomHooks.h"
#include <stdio.h>
#include <iostream>

#include <fstream>
#include <string>
#include <windows.h>
#include <regex>
#include <vector>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

const std::string empireNameRegex = "^\".+\"\=";
const std::string customEmpireDesignsFile = "C:\\Users\\alexa\\Documents\\Paradox Interactive\\Stellaris\\user_empire_designs_v3.4.txt";

void SetupHooks()
{
	// Create a console for Debug output
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	std::cout << "Hello world\n";


    std::ifstream file(customEmpireDesignsFile);
    if (!file) {
        std::cout << "Failed to read file\n";
        return;
    }
    std::cout << "customs_empire_designs read\n";

    std::string str;
    std::regex empireNames(empireNameRegex);
    std::vector<std::string> empires;

    while (std::getline(file, str))
    {
        if (std::regex_search(str, empireNames)) {
            auto words_begin = std::sregex_iterator(str.begin(), str.end(), empireNames);
            auto words_end = std::sregex_iterator();

            for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            {
                std::smatch match = *i;
                std::string match_str = match.str();
                empires.push_back(match_str);
                std::cout << "Found empire: " << match_str << "\n";
            }
        }
    }

    std::string githubEmpires = GetGithubCustomEmpires();
    while (std::getline(file, str))
    {
        if (std::regex_search(str, empireNames)) {
            auto words_begin = std::sregex_iterator(str.begin(), str.end(), empireNames);
            auto words_end = std::sregex_iterator();

            for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            {
                std::smatch match = *i;
                std::string match_str = match.str();
                empires.push_back(match_str);
                std::cout << "Found empire: " << match_str << "\n";
            }
        }
    }



	// Setup hooks here, see examples below
}

std::string GetGithubCustomEmpires() {
    HINTERNET hSession = WinHttpOpen(L"WinHTTP Example/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    HINTERNET hConnect = WinHttpConnect(hSession, L"https://github.com/Rondrio/stellaris-empire-syncer/blob/main/custom-empires.txt",
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    
    if (!hConnect)
    {
        std::wcerr << L"Fehler bei WinHttpConnect: " << GetLastError() << std::endl;
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Erstelle den Request
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/",
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    if (!hRequest)
    {
        std::wcerr << L"Fehler bei WinHttpOpenRequest: " << GetLastError() << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Sende den Request
    BOOL bResults = WinHttpSendRequest(hRequest,
        L"User-Agent: WinHTTP Example/1.0",
        -1, WINHTTP_NO_REQUEST_DATA, 0,
        0, 0);

    if (!bResults)
    {
        std::wcerr << L"Fehler bei WinHttpSendRequest: " << GetLastError() << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Empfange die Antwort
    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults)
    {
        std::wcerr << L"Fehler bei WinHttpReceiveResponse: " << GetLastError() << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Lies die Antwortdaten
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    std::string responseData;

    do
    {
        // Ermittle die Größe der verfügbaren Daten
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
        {
            std::wcerr << L"Fehler bei WinHttpQueryDataAvailable: " << GetLastError() << std::endl;
            break;
        }

        if (!dwSize)
            break;

        // Alloziere Speicher für die Daten
        pszOutBuffer = new char[dwSize + 1];
        if (!pszOutBuffer)
        {
            std::wcerr << L"Speicherfehler" << std::endl;
            break;
        }

        // Lies die Daten
        ZeroMemory(pszOutBuffer, dwSize + 1);
        if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
        {
            std::wcerr << L"Fehler bei WinHttpReadData: " << GetLastError() << std::endl;
        }
        else
        {
            responseData.append(pszOutBuffer, dwDownloaded);
        }

        // Gib den Speicher frei
        delete[] pszOutBuffer;
    } while (dwSize > 0);


    // Räume auf
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return responseData;
}