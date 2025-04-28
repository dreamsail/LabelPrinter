#include "LabelPrintThread.h"
#include "CustomWM.h"
void LabelPrintThread::Release(){
	this->~LabelPrintThread();
	delete this;
}

void LabelPrintThread::Run()
{
	//已经发送打印列表
	std::vector<ILabel*>sendList;
	ILabel* currentLabel = NULL;
	PrintDeviceState state;
	EnterCriticalSection(&this->criticalSection);
	while (true) {
		//如果标签列表是空或者设备未打开就等待唤醒
		while (this->labelList.empty()|| this->deviceHandle == NULL){
			SleepConditionVariableCS(&this->conditionVariable, &this->criticalSection, INFINITE);
		}

		//取下一个label
		currentLabel = this->labelList.front();

		 state = this->GetDeviceState();
		//打印机有无法继续打印的状态 排除0x20打印中状态
		if (state.a &(~0x20)) {
			//发送错误状态
			PostMessage(this->hwnd, WM_PRINT_DEVICE_STATE, (LPARAM)state.c, (LPARAM)currentLabel);
			//释放一会 不能一直占着临界区 会导致无法插入新的内容
			SleepConditionVariableCS(&this->conditionVariable, &this->criticalSection, 500);
			continue;
		}
		//发送任务指令
		char* tsplCommand = currentLabel->GetTsplCommand();
		WriteFile(
			this->deviceHandle,
			tsplCommand,
			strlen(tsplCommand),
			NULL,
			&this->ov
		);
		if (WaitForSingleObject(this->ov.hEvent, 1000) == WAIT_TIMEOUT){
			//无响应
			PostMessage(this->hwnd, WM_PRINT_DEVICE_STATE, (LPARAM)0x100, (LPARAM)currentLabel);
			continue;
		}
		//加入已发送批次列表
		sendList.push_back(currentLabel);
		//发送正在打印状态
		PostMessage(this->hwnd, WM_PRINT_DEVICE_STATE, (LPARAM)0x20, (LPARAM)currentLabel);
		//出队列
		this->labelList.pop();

		//已发送任务到达10条或者队列已经空 等待该批次任务完成
		if (sendList.size()>=10|| this->labelList.empty()){
			//释放一秒 让打印机更新一下状态
			SleepConditionVariableCS(&this->conditionVariable, &this->criticalSection, 1000);
			//再次判断一下  万一等待的时候有新任务下发呢？ (●'◡'●)
			if (sendList.size() >= 10 || this->labelList.empty()) {
				while (TRUE) {
					//查询当前打印机状态
					state = this->GetDeviceState();
					for (auto label : sendList) {
						//发送状态
						PostMessage(this->hwnd, WM_PRINT_DEVICE_STATE, (LPARAM)state.c, (LPARAM)label);
					}
					//打印机状态就绪
					if (state.c == 0) {
						//清空已发送任务列表
						sendList.clear();
						break;
					}
					else {
						//发送状态
						PostMessage(this->hwnd, WM_PRINT_DEVICE_STATE, (LPARAM)state.c, (LPARAM)currentLabel);
					}
					//释放一会 不能一直占着临界区 会导致无法插入新的任务
					//让新的任务有机会下发，还有就是让打印机休息，避免频繁的查询打印机状态
					SleepConditionVariableCS(&this->conditionVariable, &this->criticalSection, 300);
				}
			}
		}
	}
	LeaveCriticalSection(&this->criticalSection);
}

void LabelPrintThread::SetHandle(HANDLE handle){
	this->handle = handle;
}

HANDLE LabelPrintThread::GetHandle(){
    return this->handle;
}

PrintDeviceState LabelPrintThread::GetDeviceState()
{
	char command[] = { 0x1b,0x21,0x3f };
	PrintDeviceState state;
	WriteFile(
		this->deviceHandle,
		command,
		3,
		NULL,
		&this->ov
	);
	if (WaitForSingleObject(this->ov.hEvent, 1000) == WAIT_TIMEOUT) {
		state.b.i = 1;
		return state;
	}
	ReadFile(this->deviceHandle, &state.a, sizeof(state.a), NULL, &this->ov);
	if (WaitForSingleObject(this->ov.hEvent, 1000) == WAIT_TIMEOUT){
		state.b.i=1;
		return state;
	}

	return state;
}

LabelPrintThread::LabelPrintThread()
{
	this->handle = NULL;
	this->deviceHandle = NULL;
	this->hwnd = NULL;

	InitializeCriticalSection(&this->criticalSection);
	InitializeConditionVariable(&this->conditionVariable);
	this->ov = { 0 };
	this->ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

LabelPrintThread::~LabelPrintThread()
{
	while (!this->labelList.empty()) {
		this->labelList.front()->Release();
		this->labelList.pop();
	}
	DeleteCriticalSection(&this->criticalSection);
	CloseHandle(this->handle);
	CloseHandle(this->ov.hEvent);
}

void LabelPrintThread::SetDeviceHandle(HANDLE handle){
	EnterCriticalSection(&this->criticalSection);
	this->deviceHandle = handle;
	WakeConditionVariable(&this->conditionVariable);
	LeaveCriticalSection(&this->criticalSection);
}

HANDLE LabelPrintThread::GetDeviceHandle(){
	return this->deviceHandle;
}

void LabelPrintThread::SetHwnd(HWND hwnd){
	this->hwnd = hwnd;
}

HWND LabelPrintThread::GetHwnd(){
	return this->hwnd;
}

void LabelPrintThread::PushLabel(ILabel* label)
{
	EnterCriticalSection(&this->criticalSection);
	this->labelList.push(label);
	WakeConditionVariable(&this->conditionVariable);
	LeaveCriticalSection(&this->criticalSection);
}
