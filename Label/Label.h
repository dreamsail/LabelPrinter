#pragma once
#ifndef LABEL_H
#define LABEL_H

#include <windows.h>
#include <vector>
#include <memory>
#include <json/json.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WINDLL 
#define DLL_API __declspec(dllexport)
#else 
#ifdef _DLL
#define DLL_API __declspec(dllimport)
#else
#define DLL_API
#endif  //_DLL
#endif //_WINDLL


    // 枚举定义（需放在所有类之前）
    enum class LabelFont {
        TSS16, TSS20, TSS24, TSS32, TST24,
    };

    enum class ECCLevel { L, M, Q, H };

    enum class LabelLayout {
        Center,
        Left,
        Right,
        Top,
        Bottom,
    };
    enum class LabelPrintState {
        Print_Wait,
        Print_Error,
        Print_Success,
    };

    class ILabelElement {
    public:
        virtual char* GetType() = 0;
        virtual void Release() = 0;
        virtual char* GetTsplCommand() = 0;
    };

    class ILabel
    {
    public:
        virtual void Release() = 0;

        virtual void SetTitle(char* title) = 0;
        virtual char* GetTitle() = 0;

        virtual void SetState(LabelPrintState state) = 0;
        virtual LabelPrintState GetState() = 0;

        virtual void SetTime(char* time) = 0;
        virtual char* GetTime() = 0;

        virtual void SetWidth(int width) = 0;
        virtual int GetWidth() = 0;
        virtual void SetHeight(int height) = 0;
        virtual int GetHeight() = 0;
        virtual void SetGapN(int gapN) = 0;
        virtual int GetGapN() = 0;
        virtual void SetGapM(int gapM) = 0;
        virtual int GetGapM() = 0;
        virtual void SetNum(int num) = 0;
        virtual int GetNum() = 0;
        virtual char* GetTsplCommand() = 0;
    };

    DLL_API ILabel* WINAPI DSLabel_Create(Json::Value root);




#ifdef __cplusplus
}
#endif
#endif // ! LABEL_H