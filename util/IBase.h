#pragma once
#ifndef DS_IBASE_H
#define DS_IBASE_H
namespace ds {

	class IBase
	{
	public:
		virtual void Release() = 0;
	};
}

#endif // !DS_IBASE_H
