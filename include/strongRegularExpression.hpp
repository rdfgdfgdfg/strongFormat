
#pragma once

#include <vector>
#include <string>


#ifdef TUT_RE_DEBUG
#include <iostream>
#define TUT_LOG(A) std::printf A
#else
#define TUT_LOG(A)  
#endif


#define ERROR_CLASS_DEF(NAME, MESSAGE) \
class NAME : public std::runtime_error{\
	public:NAME() : std::runtime_error(MESSAGE) {};\
};

#define CLASS_SET(FUNCTION, OBJ, TYPE) \
inline void FUNCTION(TYPE a){this-> OBJ = a;}
#define CLASS_GET(FUNCTION, OBJ) \
inline auto FUNCTION() const noexcept {return this-> OBJ;}


namespace TUT {
	namespace re {
		using string = std::wstring;
		using size_t = std::size_t;

		template<class T>
		using vector = std::vector<T>;

#define VIRTUAL_TEMPLATE_DEF(CLASS_NAME, INIT_LIST) \
virtual Basic* copy() const { return new CLASS_NAME(*this); }\
virtual Basic* matchPcopy() const { return new CLASS_NAME INIT_LIST; }\
virtual void clear() { _clear(); }


#define WCHAR_PTR_MOVE ptr = const_cast<wchar_t*>(sptr);

		ERROR_CLASS_DEF(match_error, "match error");

#define BAN_REMATCH_CHECK if(banRematch) { throw match_error(); }

		class Basic {
			friend class Ptr;
		protected:
		public:
			wchar_t* ptr;//指向字符串（没有所有权）
			size_t size;
			size_t refNum;//引用次数
			bool greedy;
			bool banRematch;

			inline void _clear() { ptr = nullptr; size = 0; }
		public:
			Basic(bool greedy = false, bool banRematch = false) :
				ptr(nullptr), size(0), greedy(greedy), banRematch(banRematch), refNum(1){}

			Basic(const Basic& obj) :
				ptr(obj.ptr), size(obj.size), greedy(obj.greedy), banRematch(obj.banRematch), refNum(1) {}


			virtual void match(const wchar_t* sptr) = 0;
			virtual void rematch() = 0;
			virtual ~Basic() {}
			virtual Basic* copy() const = 0;
			virtual Basic* matchPcopy()  const = 0;//拷贝，但是只拷贝可以match的部分
			virtual void clear() = 0;//回到派生类刚构造后的状态

			inline string moveStrResult() const {
				return string(ptr, size);
			}

			void matchFull(const wchar_t* sptr) {
				match(sptr);
				while (*getEnd() != '\0') {
					rematch();
				}
			}
			CLASS_GET(getSize, size);
			CLASS_GET(getPtr, ptr);
			CLASS_GET(getGreedy, greedy);
			CLASS_SET(setGreedy, greedy, bool);
			CLASS_GET(getBanRematch, banRematch);
			CLASS_SET(setBanRematch, banRematch, bool);

			inline wchar_t* getEnd()  const noexcept { return ptr + size; }

		};


		class Ptr {
			template<class T, class ... Args>
			friend Ptr makePtr(Args ... args);
		protected:
			Basic* ptr;
			inline Ptr(Basic* _ptr) :ptr(_ptr) {}
		public:
			inline Ptr() :ptr(nullptr) {}
			inline Ptr(const Ptr& obj){
				if (this != &obj) {
					ptr = obj.ptr;
					ptr->refNum++;
				}
			}
			inline Ptr(Ptr&& obj) noexcept :ptr(obj.ptr) {
				obj.ptr = nullptr;
			}
			~Ptr() {
				if (ptr == nullptr) { return; }
				ptr->refNum--;
				if (ptr->refNum == 0) {
					delete ptr;
				}
			}
			template<class T> inline T get() const { return dynamic_cast<T>(ptr); }
			inline Basic* get() const { return ptr; }

			inline void match(const wchar_t* sptr) { ptr->match(sptr); };
			inline void rematch() { ptr->rematch(); };
			inline void matchFull(const wchar_t* sptr) { ptr->matchFull(sptr); }
			inline Ptr copy() const { return Ptr(ptr->copy()); };
			inline Ptr matchPcopy() const { return Ptr(ptr->matchPcopy()); };
			inline void clear() { ptr->clear(); };

			inline string moveStrResult() const {
				return ptr->moveStrResult();
			}

			inline size_t getSize()  const { return ptr->getSize(); }
			inline wchar_t* getPtr() const { return ptr->getPtr(); }
			inline wchar_t* getEnd() const { return ptr->getEnd(); }
			inline bool getGreedy() const { return ptr->getGreedy(); }
			inline void setGreedy(bool greedy) { ptr->setGreedy(greedy); }
			inline bool getBanRematch() const { return ptr->getBanRematch(); }
			inline void setBanRematch(bool greedy) { ptr->setBanRematch(greedy); }

			inline bool operator==(const Ptr obj) const {
				return obj.ptr == ptr;
			}
			inline bool operator!=(const Ptr obj) const {
				return ! (obj == *this);
			}
			inline void operator=(nullptr_t) {
				this->~Ptr();
				ptr = nullptr;
			}
			inline void operator=(Basic*ptr) {
				this->~Ptr();
				this->ptr = ptr;
			}
			inline void operator=(const Ptr& obj) {
				this->~Ptr();
				ptr = obj.ptr;
				ptr->refNum++;
			}
			inline void operator=(Ptr&& obj) noexcept {
				this->~Ptr();
				ptr = obj.ptr;
				obj.ptr = nullptr;
			}
		};

		template<class T, class ... Args>
		inline Ptr makePtr(Args ... args) {
			Basic* ptr = new T(args...);
			return Ptr(ptr);
		}

#define POLYMORPHIC_PTR_MAKE(FUNCTION_NAME, CLASS_NAME) \
template<class ... Args_>\
inline Ptr FUNCTION_NAME (Args_ ... args_){\
	return makePtr<CLASS_NAME, Args_...>(args_...);\
}

		POLYMORPHIC_PTR_MAKE(String_makePtr, String);
		POLYMORPHIC_PTR_MAKE(StringOr_makePtr, StringOr);
		POLYMORPHIC_PTR_MAKE(StringExcept_makePtr, StringExcept);
		POLYMORPHIC_PTR_MAKE(Or_makePtr, Or);
		POLYMORPHIC_PTR_MAKE(Vector_makePtr, Vector);
		POLYMORPHIC_PTR_MAKE(Separate_makePtr, Separate);
		POLYMORPHIC_PTR_MAKE(RepeatConstTimes_makePtr, RepeatConstTimes);
		POLYMORPHIC_PTR_MAKE(RepeatRange_makePtr, RepeatRange);

#define STRING_CLASS_DEF(CLASS_NAME) \
	class CLASS_NAME : public Basic {\
		protected:\
			string wstr;\
			inline void _clear() { Basic::_clear(); }\
		public:\
			inline CLASS_NAME(){}\
			inline CLASS_NAME(string wstr, bool banRematch = false) :wstr(wstr), Basic(false, banRematch) {}\
			CLASS_NAME(const CLASS_NAME& obj) :Basic(obj), wstr(obj.wstr) {}\
\
			CLASS_SET(set, wstr, string);\
			CLASS_GET(get, wstr);\
			virtual ~CLASS_NAME() {}\
			virtual void match(const wchar_t* sptr);\
			virtual void rematch();\
			VIRTUAL_TEMPLATE_DEF(CLASS_NAME, (wstr));\
		};

		using resVector = vector<Ptr>;


		STRING_CLASS_DEF(String);
		STRING_CLASS_DEF(StringOr);
		STRING_CLASS_DEF(StringExcept);

#define STRING_REPEAT_CLASS_DEF(CLASS_NAME) \
	class CLASS_NAME : public Basic {\
		protected:\
			string wstr;\
			size_t min;\
			size_t max;\
			inline void _clear() { Basic::_clear(); }\
			inline void _match();\
		public:\
			inline CLASS_NAME(bool greedy = false, bool banRematch = false):\
				min(0), max(0), Basic(greedy, banRematch) {}\
			inline CLASS_NAME(string wstr, size_t min, size_t max, bool greedy = false, bool banRematch = false) :\
				wstr(wstr), min(min), max(max), Basic(greedy, banRematch) {}\
			CLASS_NAME(const CLASS_NAME& obj) :Basic(obj), wstr(obj.wstr), min(obj.min), max(obj.max) {}\
\
			CLASS_SET(set, wstr, string);\
			CLASS_GET(get, wstr);\
			CLASS_SET(setMin, min, size_t);\
			CLASS_GET(getMin, min);\
			CLASS_SET(setMax, max, size_t);\
			CLASS_GET(getMax, max);\
			virtual ~CLASS_NAME() {}\
			virtual void match(const wchar_t* sptr){\
				ptr = const_cast<wchar_t*>(sptr);\
				if(greedy){\
					while((sptr[size] != '\0') & (size < max)){\
						try {\
							_match();\
						}\
						catch (match_error) {\
							break;\
						}\
						size++;\
					}\
				}\
				else{\
					while((sptr[size] != '\0') & (size < min)){\
						_match();\
						size++;\
					}\
				}\
			}\
			virtual void rematch(){\
				BAN_REMATCH_CHECK;\
				if(greedy){\
					if (size > min) {\
						size--;\
					}\
					else {\
						throw match_error();\
					}\
				}\
				else {\
					if(size < max){\
						_match();\
						size++;\
					}\
					else{\
						throw match_error();\
					}\
				}\
			}\
			VIRTUAL_TEMPLATE_DEF(CLASS_NAME, (wstr, min, max, greedy));\
		};

		STRING_REPEAT_CLASS_DEF(StringOrRepeat);
		STRING_REPEAT_CLASS_DEF(StringExceptRepeat);

		POLYMORPHIC_PTR_MAKE(StringOrRepeat_makePtr, StringOrRepeat);
		POLYMORPHIC_PTR_MAKE(StringExceptRepeat_makePtr, StringExceptRepeat);

		class Or : public Basic {//匹配任意一个值
		protected:
			resVector templated;//模板值（无所有权）
			Ptr cpdPtr;//成功匹配的指针
			resVector::iterator cpdIt;//成功匹配的指针的模板（在templated中）(当然，也无所有权）

			inline void _clear();
		public:
			inline Or(bool banRematch = false): Basic(false, banRematch) {}
			inline Or(resVector templated, bool banRematch = false) :templated(templated), Basic(false, banRematch){}
			Or(const Or& obj) : Basic(obj), templated(obj.templated),
				cpdPtr(obj.cpdPtr), cpdIt(templated.begin() + (obj.cpdIt - obj.templated.begin())) {}

			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			virtual ~Or();

			CLASS_SET(setTemplated, templated, resVector);
			CLASS_GET(getTemplated, templated);
			CLASS_GET(getMatchedPtr, cpdPtr);
			inline Ptr getMatchedOrgTemp() { return *cpdIt; }//获得match成功的ptr的源ptr（所有权本来在外部）

			VIRTUAL_TEMPLATE_DEF(Or, (templated));
		};

		class Queue : public Basic {
			friend class Next;
		protected:

			resVector result;
			inline Queue(bool greedy = false, bool banRematch = false) : Basic(greedy, banRematch) {}
			Queue(const Queue& obj) :Basic(obj) {
				result.reserve(obj.result.size());
				for (auto i : obj.result) {
					result.push_back(i.copy());
				}
			}

			inline void _clear() {
				result.clear();
				Basic::_clear();
			}

			virtual ~Queue() {
				_clear();
			}


			void push_back_templated_ptr_and_match(Ptr ptr);
			void pop_back();
			void matchRestart();

			class Next {
				wchar_t* start;
				Ptr rematchPtr;//被执行rematch()的指针，在safety模式下会调用matchPcopy。因此需要被析构
				Ptr matchPtr;//被执行match()的指针，在safety模式下会调用matchPcopy。因此需要被析构
				void whileTryRematch();
				void matchUntilEnd();

				void recursiveNext();

				inline void getRematchPtr();

				inline void getMatchPtr();

				resVector::reverse_iterator rit;
				resVector::iterator it;
				resVector::iterator matchEnd;

				resVector vec;
			public:

				Next(resVector& _vec, resVector::reverse_iterator rit) :
					vec(_vec)
				{
					this->rit = vec.rbegin() + (rit - _vec.rbegin());
					matchEnd = this->rit.base();
					if (_vec.empty()) {
						throw match_error();
					}
				}
				inline void main();
				wchar_t* get(resVector& output);//不包括it
				~Next();
			};

		public:
			CLASS_GET(getResult, result);
		};



		class Vector : public Queue {//存放序列值的盒子
		protected:
			resVector templated;//模板（不占有指针）
		public:
			Vector(bool banRematch = false): Queue(false, banRematch) {}
			Vector(resVector templated, bool banRematch = false) :templated(templated), Queue(false, banRematch){}
			Vector(const Vector& obj) : Queue(obj), templated(obj.templated) {}
			CLASS_SET(setTemplated, templated, resVector);
			CLASS_GET(getTemplated, templated);

			inline void _clear() { Queue::_clear(); }
			virtual ~Vector() { _clear(); }
			virtual void match(const wchar_t* sptr);
			virtual void rematch();


			VIRTUAL_TEMPLATE_DEF(Vector, (templated));
		};
		
		class Separate : public Queue {
		protected:
			Ptr templated;//不占有
			Ptr split;//不占有
			size_t max;//最大templated的数量
			size_t min;//最小templated的数量

			inline void _clear() { Queue::_clear(); }
		public:
			inline Separate(Ptr templated, Ptr split, size_t min, size_t max, 
				bool greedy = false, bool banRematch = false) :
				templated(templated), split(split), min(min), max(max), Queue(greedy, banRematch) {}
			inline Separate(bool greedy = false, bool banRematch = false) :
				min(0), max(0), Queue(greedy, banRematch) {}

			Separate(const Separate& obj) :
				templated(obj.templated), split(obj.split), max(obj.max), min(obj.min), Queue(obj) {}


			virtual ~Separate() { _clear(); }
			virtual void match(const wchar_t* sptr);
			virtual void rematch();

			CLASS_SET(setTemplated, templated, Ptr);
			CLASS_GET(getTemplated, templated);
			CLASS_SET(setSplit, split, Ptr);
			CLASS_GET(getSplit, split);
			CLASS_SET(setMax, max, size_t);
			CLASS_GET(getMax, max);
			CLASS_SET(setMin, min, size_t);
			CLASS_GET(getMin, min);

			VIRTUAL_TEMPLATE_DEF(Separate, (templated, split, min, max, greedy));
		};

		class Repeat : public Queue {//存放重复值的盒子
			friend class MatchAcc;
		protected:

			Ptr repeated;//不所有该指针
			Repeat(bool greedy = false, bool banRematch = false) : Queue(greedy, banRematch) {}
			Repeat(Ptr repeated, bool greedy = false, bool banRematch = false) :
				repeated(repeated), Queue(greedy, banRematch) {}
			Repeat(const Repeat& obj) : Queue(obj), repeated(obj.repeated) {}


			inline void push_back_repeated_ptr() {
				push_back_templated_ptr_and_match(repeated.matchPcopy());
			}
			inline void _clear() {
				Queue::_clear();
			}

			//void next(resVector::reverse_iterator it);//以it为起始点，反向迭代，尽快rematch(包括it)
			virtual ~Repeat() {
				_clear();
			}

		public:
			CLASS_SET(setRepeated, repeated, Ptr);
			CLASS_GET(getRepeated, repeated);
		};


		class RepeatConstTimes : public Repeat {
		protected:
			size_t times;

			inline void _clear() {
				Repeat::_clear();
			}
		public:
			RepeatConstTimes(bool banRematch = false) :times(0), Repeat(false, banRematch) {}
			RepeatConstTimes(Ptr repeated, size_t times, bool banRematch = false) :
				Repeat(repeated, false, banRematch), times(times) {}
			RepeatConstTimes(const RepeatConstTimes& obj) :Repeat(obj), times(obj.times) {}
			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			virtual ~RepeatConstTimes() {}
			VIRTUAL_TEMPLATE_DEF(RepeatConstTimes, (repeated, times));
			CLASS_SET(setTimes, times, size_t);
			CLASS_GET(getTimes, times);
		};

		class RepeatRange : public Repeat {
		protected:
			size_t min;
			size_t max;
			inline void _clear() {
				Repeat::_clear();
			}
		public:
			RepeatRange(bool greedy = false, bool banRematch = false) :
				Repeat(greedy, banRematch), min(0), max(0) {}
			RepeatRange(Ptr repeated, size_t min, size_t max, bool greedy = false, bool banRematch = false) :
				Repeat(repeated, greedy, banRematch), min(min), max(max) {}
			RepeatRange(const RepeatRange& obj) :Repeat(obj), min(obj.min), max(obj.max) {}
			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			virtual ~RepeatRange() {}
			CLASS_SET(setMax, max, size_t);
			CLASS_GET(getMax, max);
			CLASS_SET(setMin, min, size_t);
			CLASS_GET(getMin, min);
			VIRTUAL_TEMPLATE_DEF(RepeatRange, (repeated, min, max, greedy));
		};



		//-------------------------------------------------------------------------

		void StringOrRepeat::_match() {
			if (wstr.find(ptr[size]) == string::npos) {
				throw match_error();
			}
		}

		void StringExceptRepeat::_match() {
			if (wstr.find(ptr[size]) != string::npos) {
				throw match_error();
			}
		}

		void Separate::rematch() {
			BAN_REMATCH_CHECK;
			try {
				Next next(result, result.rbegin());
				next.main();
				size = next.get(result) - ptr;
			}
			catch (match_error) {
				matchRestart();//绝对不要删去
				//否则遍历不完全

				if (greedy) {
					pop_back();
					if (size != 0) {
						pop_back();
					}
				}
				else {
					try {
						if (result.size() != 0) {
							push_back_templated_ptr_and_match(split.matchPcopy());
						}
						push_back_templated_ptr_and_match(templated.matchPcopy());
					}
					catch (match_error) {
						pop_back();
						throw match_error();
					}
				}
			}
		}

		void Separate::match(const wchar_t* sptr) {
			result.reserve(min);
			ptr = const_cast<wchar_t*>(sptr);
			size = 0;
			size_t i = 0;
			while (i < min) {
				if (i != 0) {
					push_back_templated_ptr_and_match(split.matchPcopy());
				}
				push_back_templated_ptr_and_match(templated.matchPcopy());
				i++;
			}
			if (greedy) {
				for (; i < max; i++) {
					if (i != 0) {
						try {
							push_back_templated_ptr_and_match(split.matchPcopy());
						}
						catch (match_error) {
							pop_back();
							break;
						}
					}

					try {
						push_back_templated_ptr_and_match(templated.matchPcopy());
					}
					catch (match_error) {
						if (i != 0) {
							pop_back();
						}

						pop_back();
						break;
					}
				}
			}
		}

		void StringExcept::match(const wchar_t* sptr) {
			for (auto i : wstr) {
				if (i == *sptr) {
					throw match_error();
					return;
				}
			}
			ptr = const_cast<wchar_t*>(sptr);
			size = 1;
		}

		void StringExcept::rematch() { throw match_error(); }

		void StringOr::match(const wchar_t* sptr) {
			for (auto i : wstr) {
				if (i == *sptr) {
					ptr = const_cast<wchar_t*>(sptr);
					size = 1;
					return;
				}
			}
			throw match_error();
		}

		void StringOr::rematch() { throw match_error(); }

		void Or::match(const wchar_t* sptr) {
			ptr = const_cast<wchar_t*>(sptr);
			for (cpdIt = templated.begin(); cpdIt != templated.end(); cpdIt++) {
				try {
					cpdPtr = (*cpdIt).matchPcopy();
					cpdPtr.match(sptr);
					size = cpdPtr.getSize();
					return;
				}
				catch (match_error) {}
			}
			throw match_error();
		}

		inline void Or::_clear() {
			cpdIt = templated.begin();
			Basic::_clear();
		}

		void Or::rematch() {
			BAN_REMATCH_CHECK;
			try {
				cpdPtr.rematch();
			}
			catch (match_error) {
				cpdIt++;
				for (; cpdIt != templated.end(); cpdIt++) {
					try {
						cpdPtr = (*cpdIt).matchPcopy();
						cpdPtr.match(ptr);
						size = cpdPtr.getSize();
						return;
					}
					catch (match_error) {}
				}
				throw match_error();
			}
		}

		Or::~Or() {
			_clear();
		}


		void Queue::push_back_templated_ptr_and_match(Ptr obj) {
			result.push_back(obj);
			resVector workSpace(result);
			wchar_t* chfailmin = reinterpret_cast<wchar_t*>(-1);
			wchar_t* chfailmax = nullptr;
			wchar_t* chptr = getEnd();
			if (*chptr == L'\0') {
				throw match_error();
			}
			while (true) {
				if (*chptr == '\0') {
					throw match_error();
				}
				if (chptr < chfailmin) { chfailmin = chptr; }
				if (chptr > chfailmax) { chfailmax = chptr; }
				try {
					obj.match(chptr);
					break;
				}
				catch (match_error) {
					do {
					Next next(workSpace, ++workSpace.rbegin());
					next.main();
					chptr = next.get(workSpace);
					size = chptr - ptr;
					obj.clear();
					} while ((chptr >= chfailmin) & (chptr <= chfailmax));
				}
			}
			size = obj.getEnd() - ptr;
			workSpace.swap(result);
		}

		void Queue::matchRestart() {
			wchar_t* start = ptr;
			for (auto i : result) {
				i.clear();
				i.match(start);
				start = i.getEnd();
			}
			size = start - ptr;
		}

		void Queue::pop_back() {
			result.pop_back();
			if (result.empty()) {
				ptr = nullptr;
				size = 0;
			}
			else {
				size = result.back().getEnd() - ptr;
			}
		}

		void Vector::match(const wchar_t* sptr) {
			result.reserve(templated.size());
			ptr = const_cast<wchar_t*>(sptr);

			for (auto i = templated.begin(); i != templated.end(); i++) {
				push_back_templated_ptr_and_match((*i).matchPcopy());
			}
		}

		void Vector::rematch() {
			BAN_REMATCH_CHECK;
			Next next(result, result.rbegin());
			next.main();
			size = next.get(result) - ptr;
		}

		wchar_t* Queue::Next::get(resVector& outPut) {
			auto i = vec.begin();
			auto i_ = outPut.begin();
			while (i != vec.end()) {
				*i_ = *i;
				i++;
				i_++;
			}
			return start;
		}

		Queue::Next::~Next() {}

		void Queue::Next::getMatchPtr() {
			*it = (*it).matchPcopy();
			matchPtr = *it;
		}

		void Queue::Next::main() {
			whileTryRematch();
			matchUntilEnd();
		}


		void Queue::Next::matchUntilEnd() {
			TUT_LOG(("Queue::Next::matchUntilEnd->start\n"));
			for (it = rit.base(); it != matchEnd; it++) {
				//从rematch的后一个元素开始匹配，变更sptr
				getMatchPtr();
				while (true) {
					try {
						matchPtr.match(start);
						TUT_LOG(("Queue::Next::matchUntilEnd->match sucess\n"));
						break;
					}
					catch (match_error) {
						TUT_LOG(("Queue::Next::matchUntilEnd->match error\n"));
						try {
							matchPtr.clear();
							recursiveNext();
						}
						catch (match_error e) {
							//delete matchPtr;
							TUT_LOG(("Queue::Next::matchUntilEnd->fetal error\n"));
							throw e;
						}
					}
				}


				start = matchPtr.getEnd();
			}
			TUT_LOG(("Queue::Next::matchUntilEnd->end\n"));
		}

		void Queue::Next::recursiveNext() {
			TUT_LOG(("Queue::Next::recursiveNext->start\n"));
			Next next(vec, resVector::reverse_iterator(it));
			next.main();

			start = next.get(vec);
			TUT_LOG(("Queue::Next::recursiveNext->end\n"));
		}

		void Queue::Next::whileTryRematch() {
			while (rit != vec.rend()) {//倒着迭代，一直rematch，失败就继续，继续不了就报错
				try {
					getRematchPtr();
					rematchPtr.rematch();
					start = rematchPtr.getEnd();
					*rit = rematchPtr;
					return;//如果执行到这里还没有报错跳出，说明rematch成功
				}
				catch (match_error) {
					rit++;
				}
			}
			throw match_error();
		}

		void Queue::Next::getRematchPtr() {
			rematchPtr = (*rit).copy();
		}



		void RepeatRange::match(const wchar_t* sptr) {
			result.reserve(min);
			ptr = const_cast<wchar_t*>(sptr);
			size = 0;
			size_t i = 0;
			while (i < min) {
				push_back_repeated_ptr();
				i++;
			}
			if (greedy) {
				for (; i < max; i++) {
					try {
						push_back_repeated_ptr();
					}
					catch (match_error) {
						pop_back();
						break;
					}
				}
			}

		}


		void RepeatRange::rematch() {
			BAN_REMATCH_CHECK;
			try {
				Next next(result, result.rbegin());
				next.main();
				next.get(result);
			}
			catch (match_error) {
				if (greedy) {
					pop_back();
				}
				else {
					try {
						matchRestart();//不要删去
						//否则遍历不完全

						push_back_repeated_ptr();

					}
					catch (match_error) {
						pop_back();
						throw match_error();
					}
				}
			}

		}


		void RepeatConstTimes::match(const wchar_t* sptr) {
			result.reserve(times);
			wchar_t* chptr = const_cast<wchar_t*>(sptr);
			ptr = chptr;
			size_t sizeAcc = 0;//size累加器
			for (size_t i = 0; i < times; i++) {
				push_back_repeated_ptr();
			}
			size = sizeAcc;
		}

		void RepeatConstTimes::rematch() {
			BAN_REMATCH_CHECK;
			Next next(result, result.rbegin());
			next.main();
			next.get(result);

			size_t sizeAcc = 0;//size累加器
			for (auto i : result) {
				sizeAcc += i.getSize();
			}
			size = sizeAcc;
		}

		void String::match(const wchar_t* sptr) {
			auto it = wstr.begin();
			wchar_t* i = const_cast<wchar_t*>(sptr);
			size_t rsize = 0;
			while (true) {
				if (it == wstr.end()) {
					ptr = const_cast<wchar_t*>(sptr);
					size = rsize;
					return;
				}
				if (*i == L'\0') {
					throw match_error();
				}

				if (*it != *i) {
					throw match_error();
				}

				it++;
				i++;
				rsize++;
			}
		}

		void String::rematch() {
			throw match_error();
		}


	}
}