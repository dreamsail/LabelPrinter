#include "Label.h"
#include <string>
#include <format>
#include <time.h>
std::vector<std::string> wrapText(const std::string& input, int englishWidth, int chineseWidth, int rectWidth) {
    std::vector<std::string> result;
    std::string tmp_string;
    int currentLineWidth = 0;
    for (size_t i = 0; i < input.length(); )
    {

        if ((unsigned short(input[i])) > 0x7F)
        {
            //下个字符要超出范围了
            if ((currentLineWidth + chineseWidth) > rectWidth)
            {
                result.push_back(tmp_string);
                tmp_string = "";
                currentLineWidth = 0;
            }
            else {
                tmp_string += input.substr(i, 2);
                i += 2;
                currentLineWidth += chineseWidth;
            }
        }
        else {
            if ((currentLineWidth + englishWidth) > rectWidth)
            {
                result.push_back(tmp_string);
                tmp_string = "";
                currentLineWidth = 0;
            }
            else {
                tmp_string += input.substr(i, 1);
                i += 1;
                currentLineWidth += englishWidth;
            }
        }
    }//如果只有一行那么要在循环外加入
    if (tmp_string.length())
    {
        result.push_back(tmp_string);
    }
    return result;
}
int TextRowLayout(int x_start,int x_end, const std::string& input, int englishWidth, int chineseWidth, LabelLayout hLayout){
    int rowWidth = x_end - x_start;
    int textWidth = 0;
    for (size_t i = 0; i < input.length(); ){
        if ((unsigned short(input[i])) > 0x7F){
            i += 2;
            textWidth += chineseWidth;
        }
        else {
            i += 1;
            textWidth += englishWidth;
        }
    }
    int xOffset = 0;
    if (hLayout == LabelLayout::Left){
        xOffset = 0;
    }
    if (hLayout == LabelLayout::Right){
        xOffset = x_end - textWidth-x_start;
    }
    if (hLayout == LabelLayout::Center){
        xOffset = (rowWidth - textWidth) / 2;
    }
    return xOffset;
}


class LabelQRcode : public ILabelElement {
private:
    std::string type;
    int x;
    int y;
    ECCLevel eccLevel;
    int cellWidth;
    std::string text;

    std::string tsplCommand;
public:
    LabelQRcode(Json::Value item);
    char* GetType() override;
    void Release() override;
    char* GetTsplCommand() override;
};
LabelQRcode::LabelQRcode(Json::Value item) {
    this->type = "qrcode";
    if (item.isMember("x")){
        this->x = item["x"].asInt();
    }
    else {
        this->x =0;
    }
    if (item.isMember("y")) {
        this->y = item["y"].asInt();
    }
    else {
        this->y = 0;
    }
    if (item.isMember("eccLevel")) {
        this->eccLevel = (ECCLevel)item["eccLevel"].asInt();
    }
    else {
        this->eccLevel = ECCLevel::L;
    }
    if (item.isMember("cellWidth")) {
        this->cellWidth = item["cellWidth"].asInt();
    }
    else {
        this->cellWidth =1;
    }
    if (item.isMember("text")) {
        this->text = item["text"].asCString();
    }
    else {
        this->text = 1;
    }
}
char* LabelQRcode::GetType(){
    return (char*)type.data();
}
void LabelQRcode::Release(){
    delete this;
}
char* LabelQRcode::GetTsplCommand(){
    std::string sEccLevel;
    switch (this->eccLevel)
    {
    case ECCLevel::L:
        sEccLevel = "L";
        break;
    case ECCLevel::M:
        sEccLevel = "M";
        break;
    case ECCLevel::Q:
        sEccLevel = "Q";
        break;
    case ECCLevel::H:
        sEccLevel = "H";
        break;
    default:
        sEccLevel = "L";
        break;
    }
    this->tsplCommand = std::format(
        "QRCODE {},{},{},{},A,0,\"{}\"\n",
        this->x, this->y, sEccLevel, this->cellWidth,
        this->text
    );
    return this->tsplCommand.data();
}





class LabelText : public ILabelElement {
private:
    std::string type;
    int x_start;
    int y_start;
    int x_end;
    int y_end;
    LabelFont font;
    int fontWidth,fontHeight;
    int rotation;
    int xMultiplication;
    int yMultiplication;
    std::vector<std::string> texts;
    LabelLayout hLayout;
    LabelLayout vLayout;
    std::string tsplCommand;
public:
    LabelText(Json::Value item);
    char* GetType() override;
    void Release();
    char* GetTsplCommand();
   
};
LabelText::LabelText(Json::Value item) {
    type = "text";
    if (item.isMember("x_start")) {
        this->x_start = item["x_start"].asInt();
    }
    else {
        this->x_start = 0;
    }
    if (item.isMember("y_start")) {
        this->y_start = item["y_start"].asInt();
    }
    else {
        this->y_start = 0;
    }
    if (item.isMember("x_end")) {
        this->x_end = item["x_end"].asInt();
    }
    else {
        this->x_end = 0;
    }
    if (item.isMember("y_end")) {
        this->y_end = item["y_end"].asInt();
    }
    else {
        this->y_end = 0;
    }
    if (item.isMember("xMultiplication")) {
        this->xMultiplication = item["xMultiplication"].asInt();
    }
    else {
        this->xMultiplication = 1;
    }
    if (item.isMember("yMultiplication")) {
        this->yMultiplication = item["yMultiplication"].asInt();
    }
    else {
        this->yMultiplication = 1;
    }
    if (item.isMember("font")) {
        this->font = (LabelFont)item["font"].asInt();
        switch (this->font)
        {
        case LabelFont::TSS16:
            this->fontWidth = 16, this->fontHeight = 16;
            break;
        case LabelFont::TSS20:
            this->fontWidth = 20, this->fontHeight = 20;
            break;
        case LabelFont::TSS24:
            this->fontWidth = 24, this->fontHeight = 24;
            break;
        case LabelFont::TSS32:
            this->fontWidth = 32, this->fontHeight = 32;
            break;
        case LabelFont::TST24:
            this->fontWidth = 24, this->fontHeight = 24;
            break;
        default:
            this->font = LabelFont::TSS16;
            break;
        }
        this->fontWidth *= this->xMultiplication, this->fontHeight *= this->yMultiplication;
    }
    else {
        this->font = LabelFont::TSS16;
        this->fontWidth = 16, this->fontHeight = 16;
        this->fontWidth *= this->xMultiplication, this->fontHeight *= this->yMultiplication;
    }
    if (item.isMember("rotation")) {
        this->rotation = item["rotation"].asInt();
    }
    else {
        this->rotation = 0;
    }
    if (item.isMember("texts")&& item["texts"].isArray()) {
        for (auto it = item["texts"].begin(); it != item["texts"].end(); it++)
        {
            if (it->isNull()){
                continue;
            }
            std::string text = it->asCString();
            //为0不折行
            if (this->x_end==0){
                this->texts.push_back(text);
            }
            else {
                auto texts = wrapText(text, this->fontWidth / 2, this->fontWidth, this->x_end - this->x_start);
                this->texts.insert(this->texts.end(), texts.begin(), texts.end());
            }
            
        }
    }
    else {
        this->texts.clear();
    }
    if (item.isMember("hLayout")) {
        this->hLayout = (LabelLayout)item["hLayout"].asInt();
    }
    else {
        this->hLayout = LabelLayout::Left;
    }
    if (item.isMember("vLayout")) {
        this->vLayout = (LabelLayout)item["vLayout"].asInt();
    }
    else {
        this->vLayout = LabelLayout::Top;
    }
}
// 通过 LabelElementBase 继承
char* LabelText::GetType() {
    return (char*)type.data();
}
void LabelText::Release() {
    delete this;
}
char* LabelText::GetTsplCommand() {
    this->tsplCommand = "";
    int yOffset=0, xOffset=0;
    int rowsHeight = this->texts.size() * this->fontHeight;
    std::string sFont;
    switch (font)
    {
    case LabelFont::TSS16:
        sFont = "TSS16.BF2";
        break;
    case LabelFont::TSS20:
        sFont = "TSS20.BF2";
        break;
    case LabelFont::TSS24:
        sFont = "TSS24.BF2";
        break;
    case LabelFont::TSS32:
        sFont = "TSS32.BF2";
        break;
    case LabelFont::TST24:
        sFont = "TST24.BF2";
        break;
    default:
        sFont = "TSS16.BF2";
        break;
    }
    if (this->vLayout == LabelLayout::Top) {
        yOffset = 0;
    }
    if (this->vLayout == LabelLayout::Bottom) {
        yOffset = this->y_end - rowsHeight - this->y_start;
    }
    if (this->vLayout == LabelLayout::Center) {
        yOffset = ((this->y_end - this->y_start) - rowsHeight) / 2;
    }

    for (auto it =this->texts.begin(); it != this->texts.end(); it++)
    {
        std::string text = *it;
        xOffset=TextRowLayout(this->x_start, this->x_end, text, this->fontWidth / 2, this->fontWidth, this->hLayout);
        //为0的时候布局失效
        if (this->x_end == 0) {
            xOffset = 0;
        }
        if (this->y_end == 0) {
            yOffset = 0;
        }
        std::string command= std::format(
            "TEXT {},{},\"{}\",{},{},{},\"{}\"\n",
            this->x_start + xOffset,
            this->y_start + yOffset,
            sFont,
            this->rotation,
            this->xMultiplication,
            this->yMultiplication,
            text.data()
        );
        this->tsplCommand.append(command);
        yOffset += this->fontHeight;
    }
    return  this->tsplCommand.data();
}

class LabelBox : public ILabelElement {
private:
    std::string type;
    int x_start;
    int y_start;
    int x_end;
    int y_end;
    int lineWidth;
    std::string tsplCommand;
public:
    LabelBox(Json::Value item);
    char* GetType() override;
    void Release() override;
    char* GetTsplCommand() override;
};
LabelBox::LabelBox(Json::Value item){
    type = "box";
    if (item.isMember("x_start")) {
        this->x_start = item["x_start"].asInt();
    }
    else {
        this->x_start = 0;
    }
    if (item.isMember("y_start")) {
        this->y_start = item["y_start"].asInt();
    }
    else {
        this->y_start = 0;
    }
    if (item.isMember("x_end")) {
        this->x_end = item["x_end"].asInt();
    }
    else {
        this->x_end = 0;
    }
    if (item.isMember("y_end")) {
        this->y_end = item["y_end"].asInt();
    }
    else {
        this->y_end = 0;
    }
    if (item.isMember("lineWidth")) {
        this->lineWidth = item["lineWidth"].asInt();
    }
    else {
        this->lineWidth =1;
    }
}
char* LabelBox::GetType(){
    return this->type.data();
}
void LabelBox::Release(){
    delete this;
}
char* LabelBox::GetTsplCommand(){
    this->tsplCommand = "";
    this->tsplCommand = std::format(
        "BOX {},{},{},{},{}\n",
        this->x_start, this->y_start, this->x_end,  this->y_end, this->lineWidth
    );
    return this->tsplCommand.data();
}

// 主Label类 
class Label :public ILabel {
public:
    std::string title;
    std::string createTime;
    LabelPrintState state;
    int width;
    int height;
    int gapN;
    int gapM;
    int num;
    std::vector<ILabelElement*> elements;
    std::string tsplCommand;
    Label(Json::Value root);

public:
    void Release() override;
    void SetTitle(char*title) override;
    char* GetTitle() override;
    void SetTime(char* time) override;
    char* GetTime() override;
    void SetState(LabelPrintState state) override;
    LabelPrintState GetState() override;
    void SetWidth(int width) override;
    int GetWidth() override;
    void SetHeight(int height) override;
    int GetHeight() override;
    void SetGapN(int gapN) override;
    int GetGapN() override;
    void SetGapM(int gapM) override;
    int GetGapM() override;
    void SetNum(int num) override;
    int GetNum() override;
    char* GetTsplCommand() override;
};
Label::Label(Json::Value label){
    if (label.isMember("title")) {
        this->title = label["title"].asCString();
    }
    else {
        this->title = "无标题";
    }
    if (label.isMember("width")) {
        this->width = label["width"].asInt();
    }
    else {
        this->width = 0;
    }
    if (label.isMember("height")) {
        this->height = label["height"].asInt();
    }
    else {
        this->height = 0;
    }
    if (label.isMember("gapM")) {
        this->gapM = label["gapM"].asInt();
    }
    else {
        this->gapM = 2;
    }
    if (label.isMember("gapN")) {
        this->gapN = label["gapN"].asInt();
    }
    else {
        this->gapN = 2;
    }
    if (label.isMember("num")) {
        this->num = label["num"].asInt();
    }
    else {
        this->num = 1;
    }

    if (!label.isMember("elements"))return;
    auto jsonElements = label["elements"];
    if (!jsonElements.isArray())return;

    ILabelElement* element=NULL;
    for (auto it = jsonElements.begin(); it != jsonElements.end(); it++){
        const Json::Value& jsonElement = *it;
        if (!jsonElement.isMember("type"))continue;
        std::string type = jsonElement["type"].asCString();
        if (type.compare("qrcode")==0){
             element = new LabelQRcode(jsonElement);
             this->elements.push_back(element);
        }else  if (type.compare("text") == 0) {
            element = new LabelText(jsonElement);
            this->elements.push_back(element);
        }
        else  if (type.compare("box") == 0) {
             element = new LabelBox(jsonElement);
             this->elements.push_back(element);
        }
    }

    this->state = LabelPrintState::Print_Wait;
    
    auto t = time(0);
    struct tm _tm;
    localtime_s(&_tm,&t);
    this->createTime = std::format("{}-{:0>2}-{:0>2} {:0>2}:{:0>2}"
        , _tm.tm_year + 1900
        , _tm.tm_mon+1
        ,_tm.tm_mday
        , _tm.tm_hour
        , _tm.tm_min
    );
}
void Label::Release(){
    for (size_t i = 0; i < elements.size(); i++)
    {
        elements.at(i)->Release();
    }
    elements.clear();
    delete this;
}
void Label::SetTitle(char*title) {
    this->title = title;
}
char* Label::GetTitle() {
    return this->title.data();
}
void Label::SetTime(char* time) {
    this->createTime = time;
}
char* Label::GetTime() {
    return this->createTime.data();
}
void Label::SetState(LabelPrintState state) {
    this->state = state;
}
LabelPrintState Label::GetState() {
    return this->state;
}
void Label::SetWidth(int width){
    this->width = width;
}
int Label::GetWidth(){
    return  this->width;
}
void Label::SetHeight(int height){
    this->height = height;
}
int Label::GetHeight(){
    return this->height;
}
void Label::SetGapN(int gapN){
    this->gapN = gapN;
}
int Label::GetGapN(){
    return this->gapN;
}
void Label::SetGapM(int gapM){
    this->gapM = gapM;
}
int Label::GetGapM(){
    return  this->gapM;
}
void Label::SetNum(int num){
    this->num = num;
}
int Label::GetNum(){
    return  this->num;
}
char* Label::GetTsplCommand(){
    this->tsplCommand = "";
    std::string command = std::format(
        "SIZE {} mm,{} mm\n"
        "GAP {} mm,{} mm\n"
        "DIRECTION 0,0\n"
        "DENSITY 15\n"
        "CLS\n",
        this->width,this->height,
        this->gapM,this->gapN
        );
    this->tsplCommand.append(command);

    for (size_t i = 0; i < elements.size(); i++)
    {
        ILabelElement* element = elements.at(i);
        this->tsplCommand.append(element->GetTsplCommand());
    }
    this->tsplCommand.append(std::format("PRINT {}\n",this->num));
    return (char*)this->tsplCommand.data();
}

ILabel* WINAPI DSLabel_Create(Json::Value root) {
    ILabel* label = NULL;
    label = new Label(root);
    return label;
}