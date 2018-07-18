#pragma once

#include <type_traits>

namespace stl_like{

	struct base_deleter { 
		virtual ~base_deleter() = default;
		virtual void destroy(void* data) = 0;
	};

	template<class T>
	struct default_deleter {
		void operator()(T* data) {
			if (data) {
				delete data;
				data = nullptr;
			}
		}
	};

	template<class T, class D = default_deleter<T> >
	struct deleter_impl : base_deleter{
		D _deleter;
		deleter_impl(const D& deleter = D())
			: _deleter(deleter) { }

		virtual void destroy(void* data) {
			_deleter(static_cast<T*>(data));
		}
	};
	
	template<class T>
	class ptr_base {

	template<class U>
		friend class ptr_base;

	protected:
		long* _counter = nullptr;
		T* _data = nullptr;
		base_deleter* _deleter = nullptr;

	protected:

		virtual ~ptr_base() = default;
		ptr_base() = default;
		ptr_base(std::nullptr_t) { }

		ptr_base(long* counter, T* data, base_deleter* deleter)
		{
			reset(counter, data, deleter);
		}

		ptr_base(const ptr_base<T>& lvalue)
		{
			reset(lvalue);
		}

		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > >
		ptr_base(const ptr_base<U>& lvalue)
		{
			reset(lvalue);
		}
		ptr_base(ptr_base<T>&& rvalue) 
		{
			reset(std::move(rvalue));
		}

		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > >
		ptr_base(ptr_base<U>&& rvalue) 
		{
			reset(std::move(rvalue));
		}

		void reset(nullptr_t)
		{
			_data = nullptr;
			_counter = nullptr;
			_deleter = nullptr;
		}

		void reset(long* counter, T* data, base_deleter* deleter) {
			_counter = counter;
			_data = data;
			_deleter = deleter;
		}

		void reset(ptr_base<T>&& rvalue) {

			_counter = rvalue._counter;
			_data = rvalue._data;
			_deleter = rvalue._deleter;

			rvalue._data = nullptr;
			rvalue._counter = nullptr;
			rvalue._deleter = nullptr;
		}

		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > >
		void reset(ptr_base<U>&& rvalue) {

			_counter = rvalue._counter;
			_data = rvalue._data;
			_deleter = rvalue._deleter;

			rvalue._data = nullptr;
			rvalue._counter = nullptr;
			rvalue._deleter = nullptr;
		}

		void reset(const ptr_base<T>& lvalue) {

			_counter = lvalue._counter;
			_data = lvalue._data;
			_deleter = lvalue._deleter;
		}

		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > >
		void reset(const ptr_base<U>& lvalue) {

			_counter = lvalue._counter;
			_data = lvalue._data;
			_deleter = lvalue._deleter;
		}

	public:

		T* get() { return _data; }
		const T* get() const { return _data; }

		T* operator->() { return _data; }
		const T* operator->() const { return _data; }

		long use_count() const { return _counter ? *_counter : 0; }
	};


	template<class T>
	class weak_ptr;

	template<typename T>
	class shared_ptr : public ptr_base<T>{

		template<class U>
			friend class shared_ptr;
	public:
		shared_ptr() 
			: ptr_base() 
		{ }

		shared_ptr(std::nullptr_t) 
			: ptr_base() 
		{ }

		explicit shared_ptr(T *data)
		{ 
			if (!data)
			{
				ptr_base::reset(nullptr);
			}
			else
			{
				ptr_base::reset(new long(1), data, new deleter_impl<T>());
			}
		}

		template<class D>
		explicit shared_ptr(T *data, D deleter)
		{
			if (!data)
			{
				ptr_base::reset(nullptr);
			}
			else
			{
				ptr_base::reset(new long(1), data, new deleter_impl<T, D>(deleter));

			}
		}

		explicit shared_ptr(const shared_ptr<T>& lvalue)
			: ptr_base(lvalue)
		{ 
			if (_counter)(*_counter)++;
		}

		explicit shared_ptr(shared_ptr<T>&& rvalue)
			: ptr_base(std::move(rvalue)) { }

		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > >
		explicit shared_ptr(const shared_ptr<U>& lvalue)
			: ptr_base(lvalue)
		{ 
			if (_counter)(*_counter)++;
		}

		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void >  >
		explicit shared_ptr(shared_ptr<U>&& rvalue)
			: ptr_base(std::move(rvalue))
		{ }

		explicit shared_ptr(const weak_ptr<T>& weak) {
			ptr_base::reset(weak);
			if (_counter)(*_counter)++;
		}
		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > >
		explicit shared_ptr(const weak_ptr<U>& weak) {
			ptr_base::reset(weak);
			if (_counter)(*_counter)++;
		}

		shared_ptr<T>& operator=(const shared_ptr<T>& lvalue) {
			if (lvalue.get() != this->_data) {
				destroy();
				ptr_base::reset(lvalue);
				if (_counter)(*_counter)++;
			}
			return *this;
		}

		shared_ptr<T>& operator=(shared_ptr<T>&& rvalue) {
			destroy();
			ptr_base::reset(std::move(rvalue));
			return *this;
		}

		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > >
		shared_ptr<T>& operator=(const shared_ptr<U>& lvalue) {
			if (lvalue.get() != this->_data) {
				destroy();
				ptr_base::reset(lvalue);
				if (_counter)(*_counter)++;
			}
			return *this;
		}
		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > >
		shared_ptr<T>& operator=(shared_ptr<U>&& rvalue) {
			destroy();
			ptr_base::reset(std::move(rvalue));
			return *this;
		}

		void reset(T* data = nullptr) {
			destroy();
			ptr_base::reset(std::move(shared_ptr<T>(data)));
		}

		template<class D>
		void reset(T* data, D deleter) {
			destroy();
			ptr_base::reset(std::move(shared_ptr<T>(data, deleter)));
		}

		template<class D>
		D* get_deleter() {
			auto deleter = dynamic_cast<deleter_impl<T, D>*>(_deleter);
			return deleter ? &deleter->_deleter : nullptr;
		}

		~shared_ptr() {
			destroy();
		}

		private:
			void destroy() {
				if (_counter && !(--(*_counter))) {
					_deleter->destroy(_data);
					delete _counter;
					delete _deleter;
				}
			}
	};

	template<class T>
	class weak_ptr : ptr_base<T>{
	public:
		weak_ptr(const shared_ptr<T> &shared){
			ptr_base::reset(shared);
		}

		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > > 
		weak_ptr(const shared_ptr<U> &shared) {
			ptr_base::reset(shared);
		}

		weak_ptr(const weak_ptr<T>& weak) {
			ptr_base::reset(weak);
		}

		weak_ptr(weak_ptr<T>&& weak) {
			ptr_base::reset(std::move(weak));
		}

		template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value, void > >
		weak_ptr(const weak_ptr<U>& weak) {
			ptr_base::reset(weak);
		}
		template<class U, class = std::enable_if<std::is_convertible<U*, T*>::value, void >::type > 
		weak_ptr(weak_ptr<U>&& weak) {
			ptr_base::reset(std::move(weak));
		}

		void operator=(const weak_ptr<T>& weak) {
			ptr_base::reset(weak);
		}

		void operator=(weak_ptr<T>&& weak) {
			ptr_base::reset(std::move(weak));
		}

		template<class U, class = std::enable_if<std::is_convertible<U*, T*>::value, void >::type > 
		void operator=(const weak_ptr<U>& weak) {
			ptr_base::reset(weak);
		}

		template<class U, class = std::enable_if<std::is_convertible<U*, T*>::value, void >::type > 
		void operator=(weak_ptr<U>&& weak) {
			ptr_base::reset(std::move(weak));
		}

		void operator=(const shared_ptr<T>& shared) {
			ptr_base::reset(shared);
		}

		template<class U, class = std::enable_if<std::is_convertible<U*, T*>::value, void >::type > 
		void operator=(const shared_ptr<U>& shared) {
			ptr_base::reset(shared);
		}

		bool expired() const { return (use_count() == 0); }

		shared_ptr<T> lock() { return shared_ptr<T>(*this); }
	};
}
