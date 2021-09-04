#include <iostream>

#pragma once

#include <vector>
#include <string>

#define ERROR_CLASS_DEF(NAME, MESSAGE) \
class NAME : public std::runtime_error{\
	public:NAME() : std::runtime_error(MESSAGE) {};\
};

#define CLASS_SET(FUNCTION, OBJ, TYPE) \
void FUNCTION(TYPE a){this-> OBJ = a;}
#define CLASS_GET(FUNCTION, OBJ) \
auto FUNCTION(){return this-> OBJ;}

namespace TUT {
	namespace format {
		using string = std::wstring;
		using size_t = std::size_t;

		template<class T>
		using vector = std::vector<T>;

#define VIRTUAL_TEMPLATE_COPY(CLASS_NAME, INIT_LIST) \
virtual Basic* copy() { return new CLASS_NAME(*this); }\
virtual Basic* matchPcopy() { return new CLASS_NAME INIT_LIST; }\
virtual void clear() { _clear(); }

#define WCHAR_PTR_MOVE ptr = const_cast<wchar_t*>(sptr);

		ERROR_CLASS_DEF(match_error, "match error");

		

		class Basic {
			friend class Ptr;
		protected:
		public:
			wchar_t* ptr;//指向字符串（没有所有权）
			size_t size;
			size_t refNum;//引用次数
			bool greedy;

			inline void _clear() { ptr = nullptr; size = 0; }
			inline Basic* _rematchPcopy(Basic* obj) {//为matchPcopy的对象添加rematch信息
				obj->ptr = ptr;
				obj->size = size;
				return obj;
			}
		public:
			Basic(bool greedy = false) :ptr(nullptr), size(0), greedy(greedy), refNum(1){}
			Basic(const Basic& obj) :ptr(obj.ptr), size(obj.size), greedy(obj.greedy), refNum(1) {}


			virtual void match(const wchar_t* sptr) = 0;
			virtual void rematch() = 0;
			virtual ~Basic() {}
			virtual Basic* copy() = 0;
			virtual Basic* matchPcopy() = 0;//拷贝，但是只拷贝可以match的部分
			virtual void clear() = 0;//回到派生类刚构造后的状态

		public:
			wchar_t* moveStrResult() {
				wchar_t* ptrt = new wchar_t[size + 1];
				memcpy(ptrt, ptr, size * sizeof(wchar_t));
				ptrt[size] = L'\0';
				return ptrt;
			}

			inline size_t getSize() { return size; }
			inline wchar_t* getPtr() { return ptr; }
			inline wchar_t* getEnd() { return ptr + size; }
		};

		class Ptr {
		protected:
			Basic* ptr;
		public:
			Ptr() :ptr(nullptr) {}
			Ptr(Basic* _ptr) :ptr(_ptr) {}
			Ptr(const Ptr& obj) :ptr(obj.ptr) {
				ptr->refNum++;
			}
			Ptr(Ptr&& obj) noexcept :ptr(obj.ptr) {
				obj.ptr = nullptr;
			}
			~Ptr() {
				if (ptr == nullptr) { return; }
				ptr->refNum--;
				if (ptr->refNum == 0) {
					delete ptr;
				}
			}
			template<class T> T get() { return dynamic_cast<T>(ptr); }

			void match(const wchar_t* sptr) { ptr->match(sptr); };
			void rematch() { ptr->rematch(); };
			Ptr copy() { return Ptr(ptr->copy()); };
			Ptr matchPcopy() { return Ptr(ptr->matchPcopy()); };
			void clear() { ptr->clear(); };

			string moveStrResult() {
				return string(ptr->ptr, ptr->size);
			}

			inline size_t getSize() { return ptr->getSize(); }
			inline wchar_t* getPtr() { return ptr->getPtr(); }
			inline wchar_t* getEnd() { return ptr->getEnd(); }

			bool operator==(Ptr obj) {
				return obj.ptr == ptr;
			}
			bool operator!=(Ptr obj) {
				return ! (obj == *this);
			}
			void operator=(nullptr_t) {
				this->~Ptr();
				ptr = nullptr;
			}
			void operator=(Basic*ptr) {
				this->~Ptr();
				this->ptr = ptr;
			}
			void operator=(const Ptr& obj) {
				this->~Ptr();
				ptr = obj.ptr;
				ptr->refNum++;
			}
			void operator=(Ptr&& obj) {
				this->~Ptr();
				ptr = obj.ptr;
				obj.ptr = nullptr;
			}
		};

		class String : public Basic {
		protected:
			string wstr;
			inline void _clear() { Basic::_clear(); }
		public:
			String() {}
			String(string wstr) :wstr(wstr) {}
			String(const String& obj) :Basic(obj), wstr(obj.wstr) {}

			void set(string str) { wstr = str; }
			virtual ~String() {}
			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			VIRTUAL_TEMPLATE_COPY(String, (wstr));
		};

		using resVector = vector<Ptr>;
		/*class safetyVector : protected resVector {
			resVector* original;

			void clear() {
				original = nullptr;
				resVector::clear();
			}
		public:
			safetyVector() :original(nullptr) {}
			safetyVector(resVector& org) :resVector(org), original(&org){}
			safetyVector(safetyVector& obj) :resVector(obj), original(&obj){}//obj的生命周期必须更长
			safetyVector(safetyVector&& obj) :
				resVector(std::move(obj)), original(obj.original)
			{
				obj.clear();
			}
			~safetyVector() {
				if (original == nullptr) { return; }
				iterator localIt = begin();
				iterator it = original->begin();
				while (localIt != end()) {
					if (*localIt != *it) {
						delete* localIt;
					}
					localIt++;
					it++;
				}
			}
			void operator=(safetyVector& obj) {
				if (&obj == this) { return; }
				this->~safetyVector();
				original = &obj;
				resVector resV = obj;
				this->swap(resV);
			}
			void moveTo(safetyVector& obj) {
				obj.resVector::swap(*this);
				obj.original = original;
				original = nullptr;
			}
			void load() {
				if (original == nullptr) { return; }
				iterator localIt = begin();
				iterator it = original->begin();
				while (localIt != end()) {
					if (*localIt != *it) {
						delete* it;
						*it = *localIt;
					}
					localIt++;
					it++;
				}
				clear();
			}
			iterator begin() { return resVector::begin(); }
			iterator end() { return resVector::end(); }
			reverse_iterator rbegin() { return resVector::rbegin(); }
			reverse_iterator rend(){ return resVector::rend(); }
			bool empty() { return resVector::empty(); }
		};*/

		class StringOr : public Basic {
		protected:
			string wstr;
			inline void _clear() { Basic::_clear(); }
		public:
			StringOr() {}
			StringOr(string wstr) :wstr(wstr) {}
			StringOr(const StringOr& obj) :Basic(obj), wstr(obj.wstr) {}

			void set(string str) { wstr = str; }
			string get() { return wstr; }
			virtual ~StringOr() {}
			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			VIRTUAL_TEMPLATE_COPY(StringOr, (wstr));
		};

		class StringExcept : public Basic {
		protected:
			string wstr;
			inline void _clear() { Basic::_clear(); }
		public:
			StringExcept() {}
			StringExcept(string wstr) :wstr(wstr) {}
			StringExcept(const StringExcept& obj) :Basic(obj), wstr(obj.wstr) {}
			void set(string str) { wstr = str; }
			string get() { return wstr; }

			virtual ~StringExcept() {}
			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			VIRTUAL_TEMPLATE_COPY(StringExcept, (wstr));

		};


		class Or : public Basic {//匹配任意一个值
		protected:
			resVector templated;//模板值（无所有权）
			Ptr cpdPtr;//成功匹配的指针
			resVector::iterator cpdIt;//成功匹配的指针的模板（在templated中）(当然，也无所有权）

			void _clear();
		public:
			Or() : cpdPtr(nullptr) {}
			Or(resVector templated) :templated(templated), cpdPtr(nullptr) {}
			Or(const Or& obj) : Basic(obj), templated(obj.templated),
				cpdPtr(cpdPtr), cpdIt(templated.begin() + (obj.cpdIt - obj.templated.begin())) {}
			void setTemplated(resVector newTemplated) { templated = newTemplated; }
			resVector getTemplated() { return templated; }

			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			virtual ~Or();

			inline Ptr getMatchedPtr() { return cpdPtr; }//获得match成功的ptr（没有所有权）
			inline Ptr getMatchedOrgTemp() { return *cpdIt; }//获得match成功的ptr的源ptr（所有权本来在外部）

			VIRTUAL_TEMPLATE_COPY(Or, (templated));
		};

		class Queue : public Basic {
			friend class Next;
		protected:

			resVector result;
			Queue(bool greedy = false) : Basic(greedy) {}
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

			class Next {
				wchar_t* start;
				Ptr rematchPtr;//被执行rematch()的指针，在safety模式下会调用matchPcopy。因此需要被析构
				Ptr matchPtr;//被执行match()的指针，在safety模式下会调用matchPcopy。因此需要被析构
				void whileTryRematch();
				void matchUntilEnd();

				void recursiveNext();

				void getRematchPtr();

				void getMatchPtr();

				resVector::reverse_iterator rit;
				resVector::iterator it;
				resVector::iterator matchEnd;

				resVector vec;
			public:

				Next(resVector& _vec, resVector::reverse_iterator rit) :
					rematchPtr(nullptr), 
					matchPtr(nullptr), 
					vec(_vec)
				{
					this->rit = vec.rbegin() + (rit - _vec.rbegin());
					matchEnd = this->rit.base();
					if (_vec.empty()) {
						throw match_error();
					}
				}
				void main();
				wchar_t* get(resVector& output);//不包括it
				~Next();
			};

		public:
			inline resVector getResult() { return result; }
		};



		class Vector : public Queue {//存放序列值的盒子
		protected:
			resVector templated;//模板（不占有指针）
		public:
			Vector() {}
			Vector(resVector templated) :templated(templated) {}
			Vector(const Vector& obj) : Queue(obj), templated(obj.templated) {}
			void setTemplated(resVector newTemplated) { templated = newTemplated; }
			resVector getTemplated() { return templated; }

			inline void _clear() { Queue::_clear(); }
			virtual ~Vector() { _clear(); }
			virtual void match(const wchar_t* sptr);
			virtual void rematch();


			VIRTUAL_TEMPLATE_COPY(Vector, (templated));
		};

		class Separate : public Queue {
		protected:
			Ptr templated;//不占有
			Ptr split;//不占有
			size_t max;//最大templated的数量
			size_t min;//最小templated的数量
		public:
			inline Separate(Ptr templated, Ptr split, size_t min, size_t max, bool greedy = false) :
				templated(templated), split(split), min(min), max(max), Queue(greedy) {}
			inline Separate(bool greedy = false) : Separate(nullptr, nullptr, 0, 0, greedy) {}

			Separate(const Separate& obj) :
				templated(obj.templated), split(obj.split), max(obj.max), min(obj.min), Queue(obj) {}

			inline void _clear() { Queue::_clear(); }
			virtual ~Separate() { _clear(); }
			virtual void match(const wchar_t* sptr);
			virtual void rematch();

			CLASS_SET(setTemplated, templated, Ptr);
			CLASS_GET(getTemplated, templated);
			VIRTUAL_TEMPLATE_COPY(Separate, (templated, split, min, max, greedy));
		};

		class Repeat : public Queue {//存放重复值的盒子
			friend class MatchAcc;
		protected:

			Ptr repeated;//不所有该指针
			Repeat(bool greedy = false) : repeated(nullptr), Queue(greedy) {}
			Repeat(Ptr repeated, bool greedy = false) :repeated(repeated), Queue(greedy) {}
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
			Ptr getRepeated() { return repeated; }
			void setRepeated(Ptr newRepeated) { repeated = newRepeated; }
		};


		class RepeatConstTimes : public Repeat {
		protected:
			size_t times;

			inline void _clear() {
				Repeat::_clear();
			}
		public:
			RepeatConstTimes(Ptr repeated, size_t times) :Repeat(repeated), times(times) {}
			RepeatConstTimes(const RepeatConstTimes& obj) :Repeat(obj), times(obj.times) {}
			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			virtual ~RepeatConstTimes() {}
			VIRTUAL_TEMPLATE_COPY(RepeatConstTimes, (repeated, times));
		};

		class RepeatRange : public Repeat {
		protected:
			size_t min;
			size_t max;
			inline void _clear() {
				Repeat::_clear();
			}
		public:
			RepeatRange(Ptr repeated, size_t min, size_t max, bool greedy = false) :
				Repeat(repeated, greedy), min(min), max(max) {}
			RepeatRange(RepeatRange& obj) :Repeat(obj), min(obj.min), max(obj.max) {}
			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			virtual ~RepeatRange() {}
			VIRTUAL_TEMPLATE_COPY(RepeatRange, (repeated, min, max, greedy));
		};

		//-------------------------------------------------------------------------

		void Separate::rematch() {
			try {
				Next next(result, result.rbegin());
				next.main();
				size = next.get(result) - ptr;
			}
			catch (match_error) {
				if (greedy) {
					size -= result.back().getSize();
					result.pop_back();
					if (size != 0) {
						size -= result.back().getSize();
						result.pop_back();
					}
				}
				else {
					try {
						if (size != 0) {
							push_back_templated_ptr_and_match(split.matchPcopy());
						}
						push_back_templated_ptr_and_match(templated.matchPcopy());
					}
					catch (match_error) {
						result.pop_back();
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
							result.pop_back();
							break;
						}
					}

					try {
						push_back_templated_ptr_and_match(templated.matchPcopy());
					}
					catch (match_error) {
						if (i != 0) {
							result.pop_back();
						}

						result.pop_back();
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
					} while (chptr >= chfailmin and chptr <= chfailmax);
				}
			}
			size = obj.getEnd() - ptr;
			workSpace.swap(result);
		}

		void Vector::match(const wchar_t* sptr) {
			result.reserve(templated.size());
			ptr = const_cast<wchar_t*>(sptr);

			for (auto i = templated.begin(); i != templated.end(); i++) {
				push_back_templated_ptr_and_match((*i).matchPcopy());
			}
		}

		void Vector::rematch() {
			Next next(result, result.rbegin());
			next.main();
			size = next.get(result) - ptr;
		}

		wchar_t* Queue::Next::get(resVector& outPut) {
			outPut.swap(vec);
			return start;
		}

		Queue::Next::~Next() {
		}

		void Queue::Next::getMatchPtr() {
			*it = (*it).matchPcopy();
			matchPtr = *it;
		}

		void Queue::Next::main() {
			whileTryRematch();
			matchUntilEnd();
		}


		void Queue::Next::matchUntilEnd() {
			for (it = rit.base(); it != matchEnd; it++) {
				//从rematch的后一个元素开始匹配，变更sptr
				getMatchPtr();
				while (true) {
					try {
						matchPtr.match(start);
						break;
					}
					catch (match_error) {
						try {
							matchPtr.clear();
							recursiveNext();
						}
						catch (match_error e) {
							//delete matchPtr;
							throw e;
						}
					}
				}


				start = (*it).getEnd();
			}
		}

		void Queue::Next::recursiveNext() {

			Next next(vec, resVector::reverse_iterator(it));
			next.main();
			start = next.get(vec);

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
						result.pop_back();
						break;
					}
				}
			}

		}


		void RepeatRange::rematch() {
			try {
				Next next(result, result.rbegin());
				next.main();
				next.get(result);
			}
			catch (match_error) {
				if (greedy) {
					size -= result.back().getSize();
					result.pop_back();
				}
				else {
					try {
						push_back_repeated_ptr();

					}
					catch (match_error) {
						result.pop_back();
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

using namespace TUT::format;

int main() {
	String spe(L" ");
	RepeatRange rrg(&spe, 0, -1);//match 0 to inf
	StringOr comma(L",，");
	Vector vec(resVector({ &rrg, &comma ,&rrg }));
	StringExcept secpt(L",，");
	RepeatRange rrg_(&secpt, 0, -1, false);
	Separate sep(&rrg_, &vec, 0, -1, true);
	sep.match(L"kf,  a  , aac");
	//sep.match(L"aac,7");
	//sep.rematch();
	resVector resV = sep.getResult();
	for (auto i : resV) {
		std::wcout << i.moveStrResult() << std::endl;
	}


}