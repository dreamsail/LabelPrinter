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

    enum class BarCodeType {
        /*
		* 作为 Intermec 在 1974 年开发的第一个字母数字符号，Code 39 是一种可变长度的离散条形码符号系统。
        * 此外，它通常被称为 Alpha39、Code 3 of 9、Code 3/9、Type 39、USS Code 39 或 USD-3。
        * 代码 39 的名称来源于它总共只能编码 39 个字符。
        * 然而，在其最新版本中，字符集已增加到 43 个。它们由大写字母（A 到 Z）、数字（0 到 9）和一些特殊字符（-、.、$、/、 +、% 和空格）。
        * 一个附加字符（表示为“*”）用于开始和停止分隔符。
        */
        CODE39,//39

        /*
        * Code 93 由 Intermec 于 1982 年制定，以补充和改进Code 39。
        * Code 93 与 Code 39 的相似之处在于它可以通过使用两个字符的组合来表示一个字母数字字符集。
        * 不同之处在于 Code 93 是一个连续符号并产生更密集的代码。
        * 与 Code 39 的 43 个字符相比，它编码 47 个字符。
        * 其高密度和紧凑的尺寸使其标签比 Code 39 生产的条形码短约 25%。
        */
        CODE93, //93

		/*
        * Computer Identics 于 1981 年设计了 Code 128，这是另一种在工业和商店中流行的线性条形码。
        * Code 128 字符集包括数字 0 到 9、字母 AZ（大写和小写）以及所有标准 ASCII 符号和控制代码。
        * 这些代码分为三个子集 A、B 和 C。
        * 有三个单独的起始代码来指示将使用哪个子集。
        * 此外，每个子集包括控制字符以切换到条形码中间的另一个子集。
        * 子集 A 包括标准 ASCII 符号、数字、大写字母和控制代码。
        * 子集 B 包括标准 ASCII 符号、数字、大小写字母。
        * 子集 C 将两个数字（从 00 到 99 的 100 个数字对的集合）压缩到每个字符中，从而提供出色的密度。
        */
		CODE128, //128 

        /*
        * EAN-8 条码源自较长的欧洲商品编号 ( EAN-13 ) 代码。
        * EAN-8 条码的主要目的是使用尽可能少的空间。
        * EAN-8 条码包括两位或三位国家代码、五位数据中的四位（根据国家代码的长度）和一个校验和位。
        * EAN-8 符号中的数据数字标识特定产品和制造商。
        */
        EAN8,//EAN8

        /*
        * 欧洲的国际物品编号协会 (EAN) 设计了 ​​EAN-13 条码类型。大多数欧洲国家都在使用这种条形码类型。
        * EAN-13 是一个 13 位（12 个数据和一个检查）条形码标准，它是原始 12 位通用产品代码 (UPC) 系统的超集。
        * 因此，任何能够读取 EAN-13 符号的软件或硬件都应该能够自动读取 UPC-A 符号。
        * EAN-13 条形码中的 13 位数字分组如下：
        *   2 或 3 位数字系统或国家代码
        *   制造商（公司）代码或前缀的 5 或 4 位数字
        *   5位产品代码
        *   1 位校验和
        */
        EAN13,//EAN13

        /*
        * UPC-A 条码由 IBM 于 1971 年创建。如今，它几乎出现在每个消费品包装上。
        * 这包括在当地超市的货架上，以及书籍、杂志和报纸上。
        * UPC-A 对 11 位产品信息数据和一个尾随校验位进行编码，总共 12 位条形码数据。
        * 11 个产品数字进一步分为三个逻辑类别。
        *   第一个数字代表有关产品类型的信息。
        *   接下来的五位数字包含有关产品制造商的信息。
        *   最后五位数字包含有关正在编码的特定产品的信息。
        */
        UPCA, //UPCA

        /*
        * UPC-E 是 UPC-A 的变体，它通过压缩不必要的零来实现更紧凑的条码。
        * 这有助于使 UPC-E 条码的大小仅为 UPC-A 条码的一半左右。
        * 因此，UPC-E 通常可用于一个非常小的空间，其中完整的 UPC-A 条码无法合理地适应。
        */
        UPCE, //UPCA

        /*
        * MSI 代码是 Plessey 代码的变体，也称为修改后的 Plessey。
        * MSI Data Corporation 开发了 MSI 代码以提高 Plessey 代码的解码精度，这是最容易出错的条形码之一。
        * MSI 代码仅包含从 0 到 9 的数字。
        */
		MSI //MSI
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