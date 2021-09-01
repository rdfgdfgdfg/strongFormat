#pragma once

#include <vector>
#include <string>

#define ERROR_CLASS_DEF(NAME, MESSAGE) \
class NAME : public std::runtime_error{\
	public:NAME() : std::runtime_error(MESSAGE) {};\
};

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
		protected:
			wchar_t* ptr;//ָ���ַ�����û������Ȩ��
			size_t size;
			bool greedy;

			inline void _clear() { ptr = nullptr; size = 0; }
			inline Basic* _rematchPcopy(Basic* obj) {//ΪmatchPcopy�Ķ������rematch��Ϣ
				obj->ptr = ptr; 
				obj->size = size;
				return obj;
			}
		public:
			Basic(bool greedy = false) :ptr(nullptr), size(0), greedy(greedy) {}
			Basic(const Basic& obj) :ptr(obj.ptr), size(obj.size), greedy(obj.greedy) {}


			virtual void match(const wchar_t* sptr) = 0;
			virtual void rematch() = 0;
			virtual ~Basic() {}
			virtual Basic* copy() = 0;
			virtual Basic* matchPcopy() = 0;//����������ֻ��������match�Ĳ���
			virtual void clear() = 0;//�ص�������չ�����״̬

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

		using resVector = vector<Basic*>;

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


		class Or : public Basic {//ƥ������һ��ֵ
		protected:
			resVector templated;//ģ��ֵ��������Ȩ��
			Basic* cpdPtr;//�ɹ�ƥ���ָ��
			resVector::iterator cpdIt;//�ɹ�ƥ���ָ���ģ�壨��templated�У�(��Ȼ��Ҳ������Ȩ��

			void _clear();
		public:
			Or(): cpdPtr(nullptr) {}
			Or(resVector templated):templated(templated), cpdPtr(nullptr) {}
			Or(const Or& obj): Basic(obj), templated(obj.templated), 
				cpdPtr(cpdPtr), cpdIt(templated.begin() + (obj.cpdIt - obj.templated.begin())) {}
			void setTemplated(resVector newTemplated) { templated = newTemplated; }
			resVector getTemplated() { return templated; }

			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			virtual ~Or();

			inline Basic* getMatchedPtr() { return cpdPtr; }//���match�ɹ���ptr��û������Ȩ��
			inline Basic* moveMatchedPtr() {
				Basic* rt = cpdPtr;
				cpdPtr = nullptr;
				return rt;
			}
			inline Basic* getMatchedOrgTemp() { return *cpdIt; }//���match�ɹ���ptr��Դptr������Ȩ�������ⲿ��

			VIRTUAL_TEMPLATE_COPY(Or, (templated));
		};

		class Queue : public Basic {
			friend class Next;
		protected:

			resVector result;
			Queue(bool greedy = false) : Basic(greedy) {}
			Queue(const Queue& obj):Basic(obj) {
				result.reserve(obj.result.size());
				for (auto i : obj.result) {
					result.push_back(i->copy());
				}
			}

			inline void _clear() {
				for (auto i : result) {
					delete i;
				}
				result.clear();
				Basic::_clear();
			}

			virtual ~Queue() {
				_clear();
			}

			void tryMatch(Basic* iptr, resVector::reverse_iterator it, wchar_t* chptr);//����ƥ�䲢��������
			
			void push_back_templated_ptr_and_match(Basic* ptr);

			class Next {
				wchar_t* start;
				Basic* rematchPtr = nullptr;//��ִ��rematch()��ָ�룬��safetyģʽ�»����matchPcopy�������Ҫ������
				Basic* matchPtr = nullptr;//��ִ��match()��ָ�룬��safetyģʽ�»����matchPcopy�������Ҫ������
				void whileTryRematch();
				void matchUntilEnd();

				void recursiveNext();

				void getRematchPtr();
				void destroyRematchPtr();

				void getMatchPtr();

				resVector::reverse_iterator rit;
				resVector::iterator it;

				resVector vec;
			public:
				Queue* super;

				Next(Queue* super, resVector&& _vec) :
					super(super)
				{
					if (_vec.empty()) {
						throw match_error();
					}
					vec = _vec;
					rit = vec.rbegin();
				}
				void main();
				wchar_t* get(resVector::iterator it, resVector& _vec);//������it
				~Next();
			};

		public:
			inline resVector getResult() { return result; }
		};



		class Vector : public Queue {//�������ֵ�ĺ���
		protected:
			resVector templated;//ģ�壨��ռ��ָ�룩
		public:
			Vector() {}
			Vector(resVector templated) :templated(templated) {}
			Vector(const Vector& obj) : Queue(obj), templated(obj.templated) {}
			void setTemplated(resVector newTemplated) { templated = newTemplated; }
			resVector getTemplated() { return templated; }

			inline void _clear(){ Queue::_clear(); }
			virtual ~Vector() { _clear(); }
			virtual void match(const wchar_t* sptr);
			virtual void rematch();


			VIRTUAL_TEMPLATE_COPY(Vector, (templated));
		};

		class Repeat : public Queue {//����ظ�ֵ�ĺ���
			friend class MatchAcc;
		protected:

			Basic* repeated;//�����и�ָ��
			Repeat(bool greedy = false) : repeated(nullptr), Queue(greedy) {}
			Repeat(Basic* repeated, bool greedy = false) :repeated(repeated), Queue(greedy){}
			Repeat(const Repeat& obj) : Queue(obj), repeated(obj.repeated) {}


			inline void push_back_repeated_ptr() { 
				push_back_templated_ptr_and_match(repeated->matchPcopy());
			}
			inline void _clear(){
				Queue::_clear();
			}

			//void next(resVector::reverse_iterator it);//��itΪ��ʼ�㣬�������������rematch(����it)
			virtual ~Repeat() {
				_clear();
			}

		public:
			Basic* getRepeated() { return repeated; }
			void setRepeated(Basic* newRepeated) { repeated = newRepeated; }
		};


		class RepeatConstTimes : public Repeat {
		protected:
			size_t times;

			inline void _clear() {
				Repeat::_clear();
			}
		public:
			RepeatConstTimes(Basic* repeated, size_t times) :Repeat(repeated), times(times){}
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
			RepeatRange(Basic* repeated, size_t min, size_t max, bool greedy = false) :
				Repeat(repeated, greedy), min(min), max(max) {}
			RepeatRange(RepeatRange& obj) :Repeat(obj), min(obj.min), max(obj.max) {}
			virtual void match(const wchar_t* sptr);
			virtual void rematch();
			virtual ~RepeatRange() {}
			VIRTUAL_TEMPLATE_COPY(RepeatRange, (repeated, min, max, greedy));
		};

		//-------------------------------------------------------------------------

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
					cpdPtr = (*cpdIt)->matchPcopy();
					cpdPtr->match(sptr);
					size = cpdPtr->getSize();
					return;
				}
				catch (match_error) {
					delete cpdPtr;
				}
			}
			cpdPtr = nullptr;
			throw match_error();
		}

		inline void Or::_clear() {
			delete cpdPtr;
			cpdPtr = nullptr;
			cpdIt = templated.begin();
			Basic::_clear();
		}

		void Or::rematch() {
			cpdIt++;
			for (; cpdIt != templated.end(); cpdIt++) {
				try {
					cpdPtr = (*cpdIt)->matchPcopy();
					cpdPtr->match(ptr);
					size = cpdPtr->getSize();
					return;
				}
				catch (match_error) {
					delete cpdPtr;

				}
			}
			cpdPtr = nullptr;
			throw match_error();
		}

		Or::~Or() {
			_clear();
		}


		void Queue::push_back_templated_ptr_and_match(Basic* obj) {
			result.push_back(obj);
			wchar_t* chptr = getEnd();
			while (true) {
				try {
					obj->match(chptr);
					break;
				}
				catch (match_error) {
					Next next(this, resVector(result.begin(), --result.end()));
					next.main();
					chptr = next.get(--result.end(), result);
					obj->clear();
				}
			}
			size = obj->getEnd() - ptr;
		}

		void Vector::match(const wchar_t* sptr) {
			result.reserve(templated.size());
			ptr = const_cast<wchar_t*>(sptr);

			for (auto i = templated.begin(); i != templated.end(); i++) {
				push_back_templated_ptr_and_match((*i)->matchPcopy());
			}
		}

		void Vector::rematch() {
			Next next(this, resVector(result));
			next.main();
			next.get(result.end(), result);
		}

		wchar_t* Queue::Next::get(resVector::iterator it, resVector& _vec) {
			resVector::reverse_iterator rit_ = vec.rbegin();
			while (true) {
				it--;
				delete* it;
				*it = *rit_;
				if (rit_ == rit) { break; }
				rit_++;
			}
			vec.clear();
			return start;
		}

		Queue::Next::~Next() {
			if (!vec.empty()) {
				if (rit != vec.rend()) {
					for (auto it_ = std::next(rit).base(); it_ != it; it_++) {
						delete *it_;
					}
				}

			}
		}

		void Queue::Next::getMatchPtr() {
			*it = (*it)->matchPcopy();
			matchPtr = *it;
		}

		void Queue::Next::main() {
			whileTryRematch();
			matchUntilEnd();
		}


		void Queue::Next::matchUntilEnd() {
			for (it = rit.base(); it != vec.end(); it++) {
				//��rematch�ĺ�һ��Ԫ�ؿ�ʼƥ�䣬���sptr
				getMatchPtr();
				while (true) {
					try {
						matchPtr->match(start);
						break;
					}
					catch (match_error) {
						try {
							matchPtr->clear();
							recursiveNext();
						}
						catch (match_error e) {
							delete matchPtr;
							throw e;
						}
					}
				}


				start = (*it)->getEnd();
			}
		}

		void Queue::Next::recursiveNext() {
			Next next(super, resVector(vec.begin(), it));
			next.main();
			start = next.get(it, vec);
		}

		void Queue::Next::whileTryRematch() {
			while (rit != vec.rend()) {//���ŵ�����һֱrematch��ʧ�ܾͼ������������˾ͱ���
				try {
					getRematchPtr();
					rematchPtr->rematch();
					start = rematchPtr->getEnd();
					*rit = rematchPtr;
					return;//���ִ�е����ﻹû�б���������˵��rematch�ɹ�
				}
				catch(match_error){
					destroyRematchPtr();
					rit++;
				}
			}
			throw match_error();
		}

		void Queue::Next::getRematchPtr() {
			rematchPtr = (*rit)->copy();
		}

		void Queue::Next::destroyRematchPtr() {
			delete rematchPtr;
		}


		void Queue::tryMatch(Basic* iptr, resVector::reverse_iterator it, wchar_t* chptr) {
			while (true) {
				try {
					iptr->match(chptr);
					break;
				}
				catch (match_error) {
					Next next(this, resVector(result.begin(), std::next(it).base()));
					next.main();
					next.get(std::next(it).base(), result);

				}
			}
		}

		void RepeatRange::match(const wchar_t* sptr) {
			result.reserve(min);
			wchar_t* chptr = const_cast<wchar_t*>(sptr);
			ptr = chptr;
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
						delete result.back();
						result.pop_back();
						break;
					}
				}
			}
			
		}


		void RepeatRange::rematch() {
			try {
				Next next(this, resVector(result));
				next.main();
				next.get(result.end(), result);
			}
			catch (match_error) {
				if (greedy) {
					size -= result.back()->getSize();
					delete result.back();
					result.pop_back();
				}
				else {
					try {
						push_back_repeated_ptr();
					}
					catch (match_error) {
						delete result.back();
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
			size_t sizeAcc = 0;//size�ۼ���
			for (size_t i = 0; i < times; i++) {
				push_back_repeated_ptr();
			}
			size = sizeAcc;
		}

		void RepeatConstTimes::rematch() {
			Next next(this, resVector(result));
			next.main();
			next.get(result.end(), result);

			size_t sizeAcc = 0;//size�ۼ���
			for (auto i : result) {
				sizeAcc += i->getSize();
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