/*
Copyright (C) 2013 Takuya Nishida

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
THE SOFTWARE.
*/

#if !defined(VITERATOR_H_)
#define VITERATOR_H_

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786) // デバッグ情報切り捨てのワーニング
#pragma warning(disable:4284) // int *などを返すoperator->のコンパイル時のワーニング
#endif // _MSC_VER > 1000

#if defined(_MSC_VER) && _MSC_VER < 1400 && !defined(__SGI_STL) && !defined(_STLPORT_VERSION)
// Visual C++付属のSTLにあわせる
#define _VITERATOR_STL_ITERATOR(tag, t, dist, ptr, ref) std::iterator<tag, t, dist>
#else
#define _VITERATOR_STL_ITERATOR(tag, t, dist, ptr, ref) std::iterator<tag, t, dist, ptr, ref>
#endif

#if defined(_MSC_VER) && _MSC_VER < 1400
// 変換コンストラクタをテンプレートコンストラクタの特殊化と定義します。
#define _VITERATOR_CONVERTION_CONSTRUCTOR template <>
#else
#define _VITERATOR_CONVERTION_CONSTRUCTOR
#endif

template <class T>
struct _virtual_iterator_void_alternative
{
	typedef T type;
};

template <>
struct _virtual_iterator_void_alternative<void>
{
	typedef void *type;
};

/**
	バーチャルイテレータの実際の機能を提供するクラスの基底クラスとなります。
	<p>参照カウントを保持し、それにあわせて自身を自動削除します。</p>
*/
class _virtual_iterator_abstract
{
public:
	_virtual_iterator_abstract() : m_life(1) { }
	virtual ~_virtual_iterator_abstract() { }
	void incref() { ++m_life; }
	void decref() { if (--m_life == 0) delete this; }
	bool shared() { return (m_life > 1); }
private:
	_virtual_iterator_abstract(const _virtual_iterator_abstract& rhs); // noncopyable
	unsigned int m_life;
};

/**
	バーチャルイテレータが継承すべきクラスです。
	<p>_virtual_iterator_abstractの参照カウントや所有権の管理を行います。</p>
*/
template <class Abstract>
class _virtual_iterator_base
{
public:
	Abstract *delivered_abstract() const
	{
		m_abstract->incref();
		return m_abstract;
	}
protected:
	_virtual_iterator_base(Abstract *abstract) : m_abstract(abstract)
	{
	}
	_virtual_iterator_base(const _virtual_iterator_base& rhs) : m_abstract(rhs.m_abstract)
	{
		m_abstract->incref();
	}
	~_virtual_iterator_base()
	{
		m_abstract->decref();
	}
	_virtual_iterator_base& operator=(const _virtual_iterator_base& rhs)
	{
		if (m_abstract != rhs.m_abstract)
		{
			m_abstract->decref();
			m_abstract = rhs.m_abstract;
			m_abstract->incref();
		}
		return *this;
	}
	const Abstract *abstract() const
	{
		return m_abstract;
	}
	Abstract *own_abstract()
	{
		if (m_abstract->shared())
		{
			Abstract *oldabstract = m_abstract;
			m_abstract = oldabstract->clone();
			oldabstract->decref();
		}
		assert(!m_abstract->shared());
		return m_abstract;
	}
private:
	Abstract *m_abstract;
};

#ifndef NDEBUG
template <class T> const void *_virtual_iterator_get_containted_iterator_id(T *dummy = 0)
{
	static T *p;
	return &p;
}
#endif // NDEBUG

class _virtual_iterator_containted_iterator_id_checker
{
protected:
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const = 0;
	void assert_containted_iterator_id(const _virtual_iterator_containted_iterator_id_checker& rhs) const
	{
		// ここでアサートが失敗した場合、比較した両辺のバーチャルイテレータが内包する
		// イテレータの型が異なる可能性があります。
		// このような比較に対する結果は不定（メモリアクセス違反の可能性あり）ですので注意してください。
		assert(containted_iterator_id() == rhs.containted_iterator_id());
	}
#else
	void assert_containted_iterator_id(const _virtual_iterator_containted_iterator_id_checker& rhs) const
	{
	}
#endif // NDEBUG
};

/**
	virtual_input_iteratorの機能を提供する抽象クラスです。
*/
template <class T, class Ref = const T&, class Ptr = const T *>
class _virtual_input_iterator_abstract : public _virtual_iterator_abstract, public _virtual_iterator_containted_iterator_id_checker
{
public:
	virtual void increment() = 0;
	virtual Ref reference() const = 0;
	virtual Ptr pointer() const = 0;
	virtual bool equal(const _virtual_input_iterator_abstract& x) const = 0;
	virtual _virtual_input_iterator_abstract<T, Ref, Ptr> *input_clone() const = 0;
	_virtual_input_iterator_abstract<T, Ref, Ptr> *clone() const { return input_clone(); }
};

/**
	virtual_output_iteratorの機能を提供する抽象クラスです。
*/
template <class T>
class _virtual_output_iterator_abstract : public _virtual_iterator_abstract, public _virtual_iterator_containted_iterator_id_checker
{
public:
	virtual void increment() = 0;
	virtual void output(const T& x) = 0;
	virtual _virtual_output_iterator_abstract<T> *output_clone() const = 0;
	_virtual_output_iterator_abstract<T> *clone() const { return output_clone(); }
};

/**
	virtual_forward_iteratorの機能を提供する抽象クラスです。
*/
template <class T, class Ref = T&, class Ptr = T *>
class _virtual_forward_iterator_abstract : public _virtual_input_iterator_abstract<T, Ref, Ptr>
{
public:
	virtual bool equal(const _virtual_forward_iterator_abstract& x) const = 0;
	virtual _virtual_forward_iterator_abstract<T, Ref, Ptr> *forward_clone() const = 0;
	_virtual_forward_iterator_abstract<T, Ref, Ptr> *clone() const { return forward_clone(); }
};

/**
	virtual_bidirectional_iteratorの機能を提供する抽象クラスです。
*/
template <class T, class Ref = T&, class Ptr = T *>
class _virtual_bidirectional_iterator_abstract : public _virtual_forward_iterator_abstract<T, Ref, Ptr>
{
public:
	virtual void decrement() = 0;
	virtual bool equal(const _virtual_bidirectional_iterator_abstract& x) const = 0;
	virtual _virtual_bidirectional_iterator_abstract<T, Ref, Ptr> *bidirectional_clone() const = 0;
	_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> *clone() const { return bidirectional_clone(); }
};

/**
	virtual_random_access_iteratorの機能を提供する抽象クラスです。
*/
template <class T, class Ref = T&, class Ptr = T *, class Dist = ptrdiff_t>
class _virtual_random_access_iterator_abstract : public _virtual_bidirectional_iterator_abstract<T, Ref, Ptr>
{
public:
	virtual void plus_equal(Dist n) = 0;
	virtual void minus_equal(Dist n) = 0;
	virtual _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *plus(Dist n) const = 0;
	virtual _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *minus(Dist n) const = 0;
	virtual Ref brackets(Dist n) const = 0;
	virtual bool equal(const _virtual_random_access_iterator_abstract& x) const = 0;
	virtual bool less(const _virtual_random_access_iterator_abstract& x) const = 0;
	virtual Dist distance(const _virtual_random_access_iterator_abstract& x) const = 0;
	virtual _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *random_access_clone() const = 0;
	_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *clone() const { return random_access_clone(); }
};

/**
	virtual_input_iteratorの機能を、It型のイテレータにより提供します。
*/
template <class It, class T, class Ref = const T&, class Ptr = const T *>
class _virtual_input_iterator_implements : public _virtual_input_iterator_abstract<T, Ref, Ptr>
{
public:
	_virtual_input_iterator_implements(It it) : m_it(it)
	{
	}
	virtual void increment()
	{
		++m_it;
	}
	virtual Ref reference() const
	{
/*		// 基本クラスのポインタを示すバーチャルイテレータが、派生クラスのポインタを示す
		// イテレータを内包しているときに、*m_itで返された派生クラスのポインタへの参照
		// からローカルオブジェクトとして、派生クラスのポインタが作成されてしまい、
		// ローカルオブジェクトの参照を返してしまうのを防ぐため、static_castする。
		return static_cast<Ref>(*m_it);*/
		return *m_it;
	}
	virtual Ptr pointer() const
	{
/*		// 基本クラスのポインタを示すバーチャルイテレータが、派生クラスのポインタを示す
		// イテレータを内包しているときに、*m_itで返された派生クラスのポインタへの参照
		// に対して&*m_itとすることで、派生クラスのポインタへのポインタとなってしまい
		// 返り値の型であるPtrに変換できなくなるのを防ぐため、static_castする。
		return &static_cast<Ref>(*m_it);*/
		return &*m_it;
	}
	virtual bool equal(const _virtual_input_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_input_iterator_implements *>(&x)->m_it);
	}
	virtual _virtual_input_iterator_abstract<T, Ref, Ptr> *input_clone() const
	{
		return new _virtual_input_iterator_implements<It, T, Ref, Ptr>(m_it);
	}
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const
	{
		return ::_virtual_iterator_get_containted_iterator_id<It>();
	}
#endif // NDEBUG
	It m_it;
};

/**
	virtual_input_iteratorのoperator->を除く機能を、It型のイテレータにより提供します。
*/
template <class It, class T, class Ref = const T&, class Ptr = const T *>
class _virtual_input_iterator_implements_no_pointer : public _virtual_input_iterator_abstract<T, Ref, Ptr>
{
public:
	_virtual_input_iterator_implements_no_pointer(It it) : m_it(it)
	{
	}
	virtual void increment()
	{
		++m_it;
	}
	virtual Ref reference() const
	{
		return *m_it;
	}
	virtual Ptr pointer() const
	{
		assert(false);
	}
	virtual bool equal(const _virtual_input_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_input_iterator_implements_no_pointer *>(&x)->m_it);
	}
	virtual _virtual_input_iterator_abstract<T, Ref, Ptr> *input_clone() const
	{
		return new _virtual_input_iterator_implements_no_pointer<It, T, Ref, Ptr>(m_it);
	}
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const
	{
		return ::_virtual_iterator_get_containted_iterator_id<It>();
	}
#endif // NDEBUG
	It m_it;
};

/**
	virtual_output_iteratorの機能を、It型のイテレータにより提供します。
*/
template <class It, class T>
class _virtual_output_iterator_implements : public _virtual_output_iterator_abstract<T>
{
public:
	_virtual_output_iterator_implements(It it) : m_it(it)
	{
	}
	virtual void increment()
	{
		++m_it;
	}
	virtual void output(const T& x)
	{
		*m_it = x;
	}
	virtual _virtual_output_iterator_abstract<T> *output_clone() const
	{
		return new _virtual_output_iterator_implements<It, T>(m_it);
	}
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const
	{
		return ::_virtual_iterator_get_containted_iterator_id<It>();
	}
#endif // NDEBUG
	It m_it;
};

/**
	virtual_forward_iteratorの機能を、It型のイテレータにより提供します。
	<p>virtual_input_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_input_iteratorの機能が実行されます。</p>
*/
template <class It, class T, class Ref = T&, class Ptr = T *>
class _virtual_forward_iterator_implements : public _virtual_forward_iterator_abstract<T, Ref, Ptr>
{
public:
	_virtual_forward_iterator_implements(It it) : m_it(it)
	{
	}
	virtual void increment()
	{
		++m_it;
	}
	virtual Ref reference() const
	{
		return *m_it;
	}
	virtual Ptr pointer() const
	{
		return &*m_it;
	}
	virtual bool equal(const _virtual_input_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_input_iterator_implements<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_forward_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_forward_iterator_implements *>(&x)->m_it);
	}
	virtual _virtual_input_iterator_abstract<T, Ref, Ptr> *input_clone() const
	{
		return new _virtual_input_iterator_implements<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_forward_iterator_abstract<T, Ref, Ptr> *forward_clone() const
	{
		return new _virtual_forward_iterator_implements<It, T, Ref, Ptr>(m_it);
	}
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const
	{
		return ::_virtual_iterator_get_containted_iterator_id<It>();
	}
#endif // NDEBUG
	It m_it;
};

/**
	virtual_forward_iteratorのoperator->を除く機能を、It型のイテレータにより提供します。
	<p>virtual_input_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_input_iteratorの機能が実行されます。</p>
*/
template <class It, class T, class Ref = T&, class Ptr = T *>
class _virtual_forward_iterator_implements_no_pointer : public _virtual_forward_iterator_abstract<T, Ref, Ptr>
{
public:
	_virtual_forward_iterator_implements_no_pointer(It it) : m_it(it)
	{
	}
	virtual void increment()
	{
		++m_it;
	}
	virtual Ref reference() const
	{
		return *m_it;
	}
	virtual Ptr pointer() const
	{
	}
	virtual bool equal(const _virtual_input_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_input_iterator_implements_no_pointer<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_forward_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_forward_iterator_implements_no_pointer *>(&x)->m_it);
	}
	virtual _virtual_input_iterator_abstract<T, Ref, Ptr> *input_clone() const
	{
		return new _virtual_input_iterator_implements_no_pointer<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_forward_iterator_abstract<T, Ref, Ptr> *forward_clone() const
	{
		return new _virtual_forward_iterator_implements_no_pointer<It, T, Ref, Ptr>(m_it);
	}
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const
	{
		return ::_virtual_iterator_get_containted_iterator_id<It>();
	}
#endif // NDEBUG
	It m_it;
};

/**
	virtual_bidirectional_iteratorの機能を、It型のイテレータにより提供します。
	<p>virtual_input_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_input_iteratorの機能が実行されます。</p>
	<p>virtual_forward_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_forward_iteratorの機能が実行されます。</p>
*/
template <class It, class T, class Ref = T&, class Ptr = T *>
class _virtual_bidirectional_iterator_implements : public _virtual_bidirectional_iterator_abstract<T, Ref, Ptr>
{
public:
	_virtual_bidirectional_iterator_implements(It it) : m_it(it)
	{
	}
	virtual void increment()
	{
		++m_it;
	}
	virtual void decrement()
	{
		--m_it;
	}
	virtual Ref reference() const
	{
		// ここで例えば、const intからintに変換できないとのコンパイルエラーがでた場合は、
		// virtual_bidirectional_iterator<int>のコンストラクタに、std::list<int>::const_iteratorなどの
		// 定数イテレータを渡している可能性があります。
		// 定数イテレータを受け取れるのは、定数バーチャルイテレータである
		// virtual_bidirectional_iterator<const int>です。
		return *m_it;
	}
	virtual Ptr pointer() const
	{
		return &*m_it;
	}
	virtual bool equal(const _virtual_input_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_input_iterator_implements<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_forward_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_forward_iterator_implements<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_bidirectional_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_bidirectional_iterator_implements *>(&x)->m_it);
	}
	virtual _virtual_input_iterator_abstract<T, Ref, Ptr> *input_clone() const
	{
		return new _virtual_input_iterator_implements<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_forward_iterator_abstract<T, Ref, Ptr> *forward_clone() const
	{
		return new _virtual_forward_iterator_implements<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_bidirectional_iterator_abstract<T, Ref, Ptr> *bidirectional_clone() const
	{
		return new _virtual_bidirectional_iterator_implements<It, T, Ref, Ptr>(m_it);
	}
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const
	{
		return ::_virtual_iterator_get_containted_iterator_id<It>();
	}
#endif // NDEBUG
	It m_it;
};

/**
	virtual_bidirectional_iteratorのoperator->を除く機能を、It型のイテレータにより提供します。
	<p>virtual_input_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_input_iteratorの機能が実行されます。</p>
	<p>virtual_forward_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_forward_iteratorの機能が実行されます。</p>
*/
template <class It, class T, class Ref = T&, class Ptr = T *>
class _virtual_bidirectional_iterator_implements_no_pointer : public _virtual_bidirectional_iterator_abstract<T, Ref, Ptr>
{
public:
	_virtual_bidirectional_iterator_implements_no_pointer(It it) : m_it(it)
	{
	}
	virtual void increment()
	{
		++m_it;
	}
	virtual void decrement()
	{
		--m_it;
	}
	virtual Ref reference() const
	{
		// ここで例えば、const intからintに変換できないとのコンパイルエラーがでた場合は、
		// virtual_bidirectional_iterator<int>のコンストラクタに、std::list<int>::const_iteratorなどの
		// 定数イテレータを渡している可能性があります。
		// 定数イテレータを受け取れるのは、定数バーチャルイテレータである
		// virtual_bidirectional_iterator<const int>です。
		return *m_it;
	}
	virtual Ptr pointer() const
	{
	}
	virtual bool equal(const _virtual_input_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_input_iterator_implements_no_pointer<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_forward_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_forward_iterator_implements_no_pointer<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_bidirectional_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_bidirectional_iterator_implements_no_pointer *>(&x)->m_it);
	}
	virtual _virtual_input_iterator_abstract<T, Ref, Ptr> *input_clone() const
	{
		return new _virtual_input_iterator_implements_no_pointer<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_forward_iterator_abstract<T, Ref, Ptr> *forward_clone() const
	{
		return new _virtual_forward_iterator_implements_no_pointer<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_bidirectional_iterator_abstract<T, Ref, Ptr> *bidirectional_clone() const
	{
		return new _virtual_bidirectional_iterator_implements_no_pointer<It, T, Ref, Ptr>(m_it);
	}
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const
	{
		return ::_virtual_iterator_get_containted_iterator_id<It>();
	}
#endif // NDEBUG
	It m_it;
};

/**
	virtual_random_access_iteratorの機能を、It型のイテレータにより提供します。
	<p>virtual_input_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_input_iteratorの機能が実行されます。</p>
	<p>virtual_forward_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_forward_iteratorの機能が実行されます。</p>
	<p>virtual_bidirectional_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_bidirectional_iteratorの機能が実行されます。</p>
*/
template <class It, class T, class Ref = T&, class Ptr = T *, class Dist = ptrdiff_t>
class _virtual_random_access_iterator_implements : public _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist>
{
public:
	_virtual_random_access_iterator_implements(It it) : m_it(it)
	{
	}
	virtual void increment()
	{
		++m_it;
	}
	virtual void decrement()
	{
		--m_it;
	}
	virtual void plus_equal(Dist n)
	{
		m_it += n;
	}
	virtual void minus_equal(Dist n)
	{
		m_it -= n;
	}
	virtual _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *plus(Dist n) const
	{
		return new _virtual_random_access_iterator_implements<It, T, Ref, Ptr, Dist>(m_it + n);
	}
	virtual _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *minus(Dist n) const
	{
		return new _virtual_random_access_iterator_implements<It, T, Ref, Ptr, Dist>(m_it - n);
	}
	virtual Ref reference() const
	{
		// ここで例えば、const intからintに変換できないとのコンパイルエラーがでた場合は、
		// virtual_random_access_iterator<int>のコンストラクタに、std::vector<int>::const_iteratorなどの
		// 定数イテレータを渡している可能性があります。
		// 定数イテレータを受け取れるのは、定数バーチャルイテレータである
		// virtual_random_access_iterator<const int>です。
		return *m_it;
	}
	virtual Ptr pointer() const
	{
		return &*m_it;
	}
	virtual Ref brackets(Dist n) const
	{
		// ここで例えば、const intからintに変換できないとのコンパイルエラーがでた場合は、
		// virtual_random_access_iterator<int>のコンストラクタに、std::vector<int>::const_iteratorなどの
		// 定数イテレータを渡している可能性があります。
		// 定数イテレータを受け取れるのは、定数バーチャルイテレータである
		// virtual_random_access_iterator<const int>です。
		return *(m_it + n);
	}
	virtual bool equal(const _virtual_input_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_input_iterator_implements<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_forward_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_forward_iterator_implements<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_bidirectional_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_bidirectional_iterator_implements<It, T,Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_random_access_iterator_implements *>(&x)->m_it);
	}
	virtual bool less(const _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it < reinterpret_cast<const _virtual_random_access_iterator_implements *>(&x)->m_it);
	}
	virtual Dist distance(const _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it - reinterpret_cast<const _virtual_random_access_iterator_implements *>(&x)->m_it);
	}
	virtual _virtual_input_iterator_abstract<T, Ref, Ptr> *input_clone() const
	{
		return new _virtual_input_iterator_implements<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_forward_iterator_abstract<T, Ref, Ptr> *forward_clone() const
	{
		return new _virtual_forward_iterator_implements<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_bidirectional_iterator_abstract<T, Ref, Ptr> *bidirectional_clone() const
	{
		return new _virtual_bidirectional_iterator_implements<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *random_access_clone() const
	{
		return new _virtual_random_access_iterator_implements<It, T, Ref, Ptr, Dist>(m_it);
	}
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const
	{
		return ::_virtual_iterator_get_containted_iterator_id<It>();
	}
#endif // NDEBUG
	It m_it;
};

/**
	virtual_random_access_iteratorのoperator->を除く機能を、It型のイテレータにより提供します。
	<p>virtual_input_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_input_iteratorの機能が実行されます。</p>
	<p>virtual_forward_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_forward_iteratorの機能が実行されます。</p>
	<p>virtual_bidirectional_iteratorの機能も実装しており、_virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >
	から呼び出された場合には、virtual_bidirectional_iteratorの機能が実行されます。</p>
*/
template <class It, class T, class Ref = T&, class Ptr = T *, class Dist = ptrdiff_t>
class _virtual_random_access_iterator_implements_no_pointer : public _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist>
{
public:
	_virtual_random_access_iterator_implements_no_pointer(It it) : m_it(it)
	{
	}
	virtual void increment()
	{
		++m_it;
	}
	virtual void decrement()
	{
		--m_it;
	}
	virtual void plus_equal(Dist n)
	{
		m_it += n;
	}
	virtual void minus_equal(Dist n)
	{
		m_it -= n;
	}
	virtual _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *plus(Dist n) const
	{
		return new _virtual_random_access_iterator_implements_no_pointer<It, T, Ref, Ptr, Dist>(m_it + n);
	}
	virtual _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *minus(Dist n) const
	{
		return new _virtual_random_access_iterator_implements_no_pointer<It, T, Ref, Ptr, Dist>(m_it - n);
	}
	virtual Ref reference() const
	{
		// ここで例えば、const intからintに変換できないとのコンパイルエラーがでた場合は、
		// virtual_random_access_iterator<int>のコンストラクタに、std::vector<int>::const_iteratorなどの
		// 定数イテレータを渡している可能性があります。
		// 定数イテレータを受け取れるのは、定数バーチャルイテレータである
		// virtual_random_access_iterator<const int>です。
		return *m_it;
	}
	virtual Ptr pointer() const
	{
	}
	virtual Ref brackets(Dist n) const
	{
		// ここで例えば、const intからintに変換できないとのコンパイルエラーがでた場合は、
		// virtual_random_access_iterator<int>のコンストラクタに、std::vector<int>::const_iteratorなどの
		// 定数イテレータを渡している可能性があります。
		// 定数イテレータを受け取れるのは、定数バーチャルイテレータである
		// virtual_random_access_iterator<const int>です。
		return *(m_it + n);
	}
	virtual bool equal(const _virtual_input_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_input_iterator_implements_no_pointer<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_forward_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_forward_iterator_implements_no_pointer<It, T, Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_bidirectional_iterator_abstract<T, Ref, Ptr>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_bidirectional_iterator_implements_no_pointer<It, T,Ref, Ptr> *>(&x)->m_it);
	}
	virtual bool equal(const _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it == reinterpret_cast<const _virtual_random_access_iterator_implements_no_pointer *>(&x)->m_it);
	}
	virtual bool less(const _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it < reinterpret_cast<const _virtual_random_access_iterator_implements_no_pointer *>(&x)->m_it);
	}
	virtual Dist distance(const _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist>& x) const
	{
		assert_containted_iterator_id(x);
		return (m_it - reinterpret_cast<const _virtual_random_access_iterator_implements_no_pointer *>(&x)->m_it);
	}
	virtual _virtual_input_iterator_abstract<T, Ref, Ptr> *input_clone() const
	{
		return new _virtual_input_iterator_implements_no_pointer<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_forward_iterator_abstract<T, Ref, Ptr> *forward_clone() const
	{
		return new _virtual_forward_iterator_implements_no_pointer<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_bidirectional_iterator_abstract<T, Ref, Ptr> *bidirectional_clone() const
	{
		return new _virtual_bidirectional_iterator_implements_no_pointer<It, T, Ref, Ptr>(m_it);
	}
	virtual _virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *random_access_clone() const
	{
		return new _virtual_random_access_iterator_implements_no_pointer<It, T, Ref, Ptr, Dist>(m_it);
	}
#ifndef NDEBUG
	virtual const void *containted_iterator_id() const
	{
		return ::_virtual_iterator_get_containted_iterator_id<It>();
	}
#endif // NDEBUG
	It m_it;
};

template <class Ptr>
struct _virtual_input_iterator_factory
{
	template <class It, class T, class Ref>
	struct abstract
	{
		typedef _virtual_input_iterator_implements<It, T, Ref, Ptr> type;
	};
};

template <>
struct _virtual_input_iterator_factory<void>
{
	template <class It, class T, class Ref>
	struct abstract
	{
		typedef _virtual_input_iterator_implements_no_pointer<It, T, Ref, void> type;
	};
};

template <class Ptr>
struct _virtual_forward_iterator_factory
{
	template <class It, class T, class Ref>
	struct abstract
	{
		typedef _virtual_forward_iterator_implements<It, T, Ref, Ptr> type;
	};
};

template <>
struct _virtual_forward_iterator_factory<void>
{
	template <class It, class T, class Ref>
	struct abstract
	{
		typedef _virtual_forward_iterator_implements_no_pointer<It, T, Ref, void> type;
	};
};

template <class Ptr>
struct _virtual_bidirectional_iterator_factory
{
	template <class It, class T, class Ref>
	struct abstract
	{
		typedef _virtual_bidirectional_iterator_implements<It, T, Ref, Ptr> type;
	};
};

template <>
struct _virtual_bidirectional_iterator_factory<void>
{
	template <class It, class T, class Ref>
	struct abstract
	{
		typedef _virtual_bidirectional_iterator_implements_no_pointer<It, T, Ref, void> type;
	};
};

template <class Ptr>
struct _virtual_random_access_iterator_factory
{
	template <class It, class T, class Ref>
	struct abstract
	{
		typedef _virtual_random_access_iterator_implements<It, T, Ref, Ptr> type;
	};
};

template <>
struct _virtual_random_access_iterator_factory<void>
{
	template <class It, class T, class Ref>
	struct abstract
	{
		typedef _virtual_random_access_iterator_implements_no_pointer<It, T, Ref, void> type;
	};
};

/**
	ランダムアクセス反復子を定義します。
	<p>このクラスは次の条件を満たすようなイテレータItを内包します。</p>
	<ul>
	<li>std::iterator_traits<It>::value_type 型が T型</li>
	<li>std::iterator_traits<It>::reference_type 型が Ref型</li>
	<li>std::iterator_traits<It>::pointer_type 型が Ptr型</li>
	<li>std::iterator_traits<It>::distance_type 型が Dist型</li>
	</ul>
	<p>このクラスは、T, Ref, Ptr, Distのみでパラメータ化され、Itによるパラメータ化はされませんので、この条件を満たすあらゆるItを、同じ型のイテレータとして内包できます。</p>
	<p>定数イテレータを作成したい場合は、テンプレートパラメータTにconst型を指定してください。</p>
*/
template <class T, class Ref = T&, class Ptr = T *, class Dist = ptrdiff_t>
class virtual_random_access_iterator : public _VITERATOR_STL_ITERATOR(std::random_access_iterator_tag, T, Dist, Ptr, Ref), public _virtual_iterator_base<_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> >
{
public:
	/**
		未定義のイテレータを内包するバーチャルイテレータを作成します。
	*/
	virtual_random_access_iterator() : _virtual_iterator_base<_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> >(
		new _virtual_random_access_iterator_factory<Ptr>::abstract<T *, T, Ref>::type(0)
		)
	{
	}
	/**
		It型のイテレータrhsを内包するバーチャルイテレータを作成します。
		@param It
			イテレータの型。
		@param rhs
			イテレータのインスタンス。
	*/
	template <class It>
	virtual_random_access_iterator(const It& rhs) : _virtual_iterator_base<_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> >(
		new _virtual_random_access_iterator_factory<Ptr>::abstract<It, T, Ref>::type(rhs)
		)
	{
	}
	/**
		バーチャルイテレータrhsと同じイテレータを作成するコピーコンストラクタです。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_random_access_iterator(const virtual_random_access_iterator& rhs) : _virtual_iterator_base<_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> >(
		rhs
		)
	{
	}
#if defined(_MSC_VER)
	~virtual_random_access_iterator()
	{
	}
	virtual_random_access_iterator& operator=(const virtual_random_access_iterator& rhs)
	{
		_virtual_iterator_base<_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> >::operator=(rhs);
		return *this;
	}
#endif
	Ref operator*() const
	{
		return abstract()->reference();
	}
	typename _virtual_iterator_void_alternative<Ptr>::type operator->() const
	{
		return abstract()->pointer();
	}
	Ref operator[](Dist n) const
	{
		return abstract()->brackets(n);
	}
	virtual_random_access_iterator& operator++()
	{
		own_abstract()->increment();
		return *this;
	}
	virtual_random_access_iterator operator++(int)
	{
		virtual_random_access_iterator old = *this;
		own_abstract()->increment();
		return old;
	}
	virtual_random_access_iterator& operator--()
	{
		own_abstract()->decrement();
		return *this;
	}
	virtual_random_access_iterator operator--(int)
	{
		virtual_random_access_iterator old = *this;
		own_abstract()->decrement();
		return old;
	}
	virtual_random_access_iterator& operator+=(Dist n)
	{
		own_abstract()->plus_equal(n);
		return *this;
	}
	virtual_random_access_iterator& operator-=(Dist n)
	{
		own_abstract()->minus_equal(n);
		return *this;
	}
	virtual_random_access_iterator operator+(Dist n) const
	{
		return virtual_random_access_iterator<T, Ref, Ptr, Dist>(abstract()->plus(n), 0);
	}
	virtual_random_access_iterator operator-(Dist n) const
	{
		return virtual_random_access_iterator<T, Ref, Ptr, Dist>(abstract()->minus(n), 0);
	}
	friend static inline bool operator==(const virtual_random_access_iterator& lhs, const virtual_random_access_iterator& rhs)
	{
		return lhs.abstract()->equal(*rhs.abstract());
	}
	friend static inline bool operator!=(const virtual_random_access_iterator& lhs, const virtual_random_access_iterator& rhs)
	{
		return !operator==(lhs, rhs);
	}
	friend static inline bool operator<(const virtual_random_access_iterator& lhs, const virtual_random_access_iterator& rhs)
	{
		return lhs.abstract()->less(*rhs.abstract());
	}
	friend static inline bool operator>(const virtual_random_access_iterator& lhs, const virtual_random_access_iterator& rhs)
	{
		return operator<(rhs, lhs);
	}
	friend static inline bool operator<=(const virtual_random_access_iterator& lhs, const virtual_random_access_iterator& rhs)
	{
		return !operator<(rhs, lhs);
	}
	friend static inline bool operator>=(const virtual_random_access_iterator& lhs, const virtual_random_access_iterator& rhs)
	{
		return !operator<(lhs, rhs);
	}
	friend static inline Dist operator-(const virtual_random_access_iterator& lhs, const virtual_random_access_iterator& rhs)
	{
		return lhs.abstract()->distance(*rhs.abstract());
	}
private:
	virtual_random_access_iterator(_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> *abstract, int) : _virtual_iterator_base<_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> >(
		abstract
		)
	{
	}
};

/**
	双方向反復子を定義します。
	<p>このクラスは次の条件を満たすようなイテレータItを内包します。</p>
	<ul>
	<li>std::iterator_traits<It>::value_type 型が T型</li>
	<li>std::iterator_traits<It>::reference_type 型が Ref型</li>
	<li>std::iterator_traits<It>::pointer_type 型が Ptr型</li>
	<li>std::iterator_traits<It>::distance_type 型が Dist型</li>
	</ul>
	<p>このクラスは、T, Ref, Ptr, Distのみでパラメータ化され、Itによるパラメータ化はされませんので、この条件を満たすあらゆるItを、同じ型のイテレータとして内包できます。</p>
	<p>定数イテレータを作成したい場合は、テンプレートパラメータTにconst型を指定してください。</p>
*/
template <class T, class Ref = T&, class Ptr = T *, class Dist = ptrdiff_t>
class virtual_bidirectional_iterator : public _VITERATOR_STL_ITERATOR(std::bidirectional_iterator_tag, T, Dist, Ptr, Ref), public _virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >
{
public:
	/**
		未定義のイテレータを内包するバーチャルイテレータを作成します。
	*/
	virtual_bidirectional_iterator() : _virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_bidirectional_iterator_factory<Ptr>::abstract<T *, T, Ref>::type(0)
		)
	{
	}
	/**
		It型のイテレータrhsを内包するバーチャルイテレータを作成します。
		@param It
			イテレータの型。
		@param rhs
			イテレータのインスタンス。
	*/
	template <class It>
	virtual_bidirectional_iterator(const It& rhs) : _virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_bidirectional_iterator_factory<Ptr>::abstract<It, T, Ref>::type(rhs)
		)
	{
	}
	/**
		バーチャルイテレータrhsと同じイテレータを作成するコピーコンストラクタです。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_bidirectional_iterator(const virtual_bidirectional_iterator& rhs) : _virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >(
		rhs
		)
	{
	}
	// 高機能なカテゴリのバーチャルイテレータで低機能なカテゴリのバーチャルイテレータへの
	// 変換メンバ関数を定義しないで、低機能側の変換コンストラクタが高機能側のバーチャルイテレータを
	// 引数にとるようにしているのは、低機能側のバーチャルイテレータを引数にとるような関数に
	// 高機能側のバーチャルイテレータを渡すとき、低機能側の無駄なコピーを発生させないためである
	/**
		バーチャルイテレータrhsが内包するイテレータを内包するバーチャルイテレータを作成します。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_bidirectional_iterator(const virtual_random_access_iterator<T, Ref, Ptr, Dist>& rhs) : _virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >(
		rhs.delivered_abstract()
		)
	{
	}
#if defined(_MSC_VER)
	~virtual_bidirectional_iterator()
	{
	}
	virtual_bidirectional_iterator& operator=(const virtual_bidirectional_iterator& rhs)
	{
		_virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >::operator=(rhs);
		return *this;
	}
#endif
	Ref operator*() const
	{
		return abstract()->reference();
	}
	typename _virtual_iterator_void_alternative<Ptr>::type operator->() const
	{
		return abstract()->pointer();
	}
	virtual_bidirectional_iterator& operator++()
	{
		own_abstract()->increment();
		return *this;
	}
	virtual_bidirectional_iterator operator++(int)
	{
		virtual_bidirectional_iterator old = *this;
		own_abstract()->increment();
		return old;
	}
	virtual_bidirectional_iterator& operator--()
	{
		own_abstract()->decrement();
		return *this;
	}
	virtual_bidirectional_iterator operator--(int)
	{
		virtual_bidirectional_iterator old = *this;
		own_abstract()->decrement();
		return old;
	}
	friend static inline bool operator==(const virtual_bidirectional_iterator& lhs, const virtual_bidirectional_iterator& rhs)
	{
		return lhs.abstract()->equal(*rhs.abstract());
	}
	friend static inline bool operator!=(const virtual_bidirectional_iterator& lhs, const virtual_bidirectional_iterator& rhs)
	{
		return !operator==(lhs, rhs);
	}
};

/**
	前方反復子を定義します。
	<p>このクラスは次の条件を満たすようなイテレータItを内包します。</p>
	<ul>
	<li>std::iterator_traits<It>::value_type 型が T型</li>
	<li>std::iterator_traits<It>::reference_type 型が Ref型</li>
	<li>std::iterator_traits<It>::pointer_type 型が Ptr型</li>
	<li>std::iterator_traits<It>::distance_type 型が Dist型</li>
	</ul>
	<p>このクラスは、T, Ref, Ptr, Distのみでパラメータ化され、Itによるパラメータ化はされませんので、この条件を満たすあらゆるItを、同じ型のイテレータとして内包できます。</p>
	<p>定数イテレータを作成したい場合は、virtual_input_iterator<T, Ref, Ptr, Dist>を利用してください。</p>
*/
template <class T, class Ref = T&, class Ptr = T *, class Dist = ptrdiff_t>
class virtual_forward_iterator : public _VITERATOR_STL_ITERATOR(std::forward_iterator_tag, T, Dist, Ptr, Ref), public _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
{
public:
	/**
		未定義のイテレータを内包するバーチャルイテレータを作成します。
	*/
	virtual_forward_iterator() : _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_forward_iterator_factory<Ptr>::abstract<T *, T, Ref>::type(0)
		)
	{
	}
	/**
		It型のイテレータrhsを内包するバーチャルイテレータを作成します。
		@param It
			イテレータの型。
		@param rhs
			イテレータのインスタンス。
	*/
	template <class It>
	virtual_forward_iterator(const It& rhs) : _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_forward_iterator_factory<Ptr>::abstract<It, T, Ref>::type(rhs)
		)
	{
	}
	/**
		バーチャルイテレータrhsと同じイテレータを作成するコピーコンストラクタです。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_forward_iterator(const virtual_forward_iterator& rhs) : _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >(
		rhs
		)
	{
	}
	// 高機能なカテゴリのバーチャルイテレータで低機能なカテゴリのバーチャルイテレータへの
	// 変換メンバ関数を定義しないで、低機能側の変換コンストラクタが高機能側のバーチャルイテレータを
	// 引数にとるようにしているのは、低機能側のバーチャルイテレータを引数にとるような関数に
	// 高機能側のバーチャルイテレータを渡すとき、低機能側の無駄なコピーを発生させないためである
	/**
		バーチャルイテレータrhsが内包するイテレータを内包するバーチャルイテレータを作成します。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_forward_iterator(const virtual_random_access_iterator<T, Ref, Ptr, Dist>& rhs) : _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >(
		rhs.delivered_abstract()
		)
	{
	}
	/**
		バーチャルイテレータrhsが内包するイテレータを内包するバーチャルイテレータを作成します。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_forward_iterator(const virtual_bidirectional_iterator<T, Ref, Ptr, Dist>& rhs) : _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >(
		rhs.delivered_abstract()
		)
	{
	}
#if defined(_MSC_VER)
	~virtual_forward_iterator()
	{
	}
	virtual_forward_iterator& operator=(const virtual_forward_iterator& rhs)
	{
		_virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >::operator=(rhs);
		return *this;
	}
#endif
	Ref operator*() const
	{
		return abstract()->reference();
	}
	typename _virtual_iterator_void_alternative<Ptr>::type operator->() const
	{
		return abstract()->pointer();
	}
	virtual_forward_iterator& operator++()
	{
		own_abstract()->increment();
		return *this;
	}
	virtual_forward_iterator operator++(int)
	{
		virtual_forward_iterator old = *this;
		own_abstract()->increment();
		return old;
	}
	friend static inline bool operator==(const virtual_forward_iterator& lhs, const virtual_forward_iterator& rhs)
	{
		return lhs.abstract()->equal(*rhs.abstract());
	}
	friend static inline bool operator!=(const virtual_forward_iterator& lhs, const virtual_forward_iterator& rhs)
	{
		return !operator==(lhs, rhs);
	}
};

/**
	入力反復子を定義します。
	<p>このクラスは次の条件を満たすようなイテレータItを内包します。</p>
	<ul>
	<li>std::iterator_traits<It>::value_type 型が T型</li>
	<li>std::iterator_traits<It>::reference_type 型が Ref型</li>
	<li>std::iterator_traits<It>::pointer_type 型が Ptr型</li>
	<li>std::iterator_traits<It>::distance_type 型が Dist型</li>
	</ul>
	<p>このクラスは、T, Ref, Ptr, Distのみでパラメータ化され、Itによるパラメータ化はされませんので、この条件を満たすあらゆるItを、同じ型のイテレータとして内包できます。</p>
	<p>このクラスは常に定数イテレータとなります。</p>
*/
template <class T, class Ref = const T&, class Ptr = const T *, class Dist = ptrdiff_t>
class virtual_input_iterator : public _VITERATOR_STL_ITERATOR(std::input_iterator_tag, T, Dist, Ptr, Ref), public _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
{
public:
	/**
		未定義のイテレータを内包するバーチャルイテレータを作成します。
	*/
	virtual_input_iterator() : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_input_iterator_factory<Ptr>::abstract<T *, T, Ref>::type(0)
		)
	{
	}
	/**
		It型のイテレータrhsを内包するバーチャルイテレータを作成します。
		@param It
			イテレータの型。
		@param rhs
			イテレータのインスタンス。
	*/
	template <class It>
	virtual_input_iterator(const It& rhs) : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_input_iterator_factory<Ptr>::abstract<It, T, Ref>::type(rhs)
		)
	{
	}
	/**
		バーチャルイテレータrhsと同じイテレータを作成するコピーコンストラクタです。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_input_iterator(const virtual_input_iterator& rhs) : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		rhs
		)
	{
	}
	// 高機能なカテゴリのバーチャルイテレータで低機能なカテゴリのバーチャルイテレータへの
	// 変換メンバ関数を定義しないで、低機能側の変換コンストラクタが高機能側のバーチャルイテレータを
	// 引数にとるようにしているのは、低機能側のバーチャルイテレータを引数にとるような関数に
	// 高機能側のバーチャルイテレータを渡すとき、低機能側の無駄なコピーを発生させないためである
	/**
		バーチャルイテレータrhsが内包するイテレータを内包するバーチャルイテレータを作成します。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_input_iterator(const virtual_random_access_iterator<T, Ref, Ptr, Dist>& rhs) : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		rhs.delivered_abstract()
		)
	{
	}
	/**
		バーチャルイテレータrhsが内包するイテレータを内包するバーチャルイテレータを作成します。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_input_iterator(const virtual_bidirectional_iterator<T, Ref, Ptr, Dist>& rhs) : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		rhs.delivered_abstract()
		)
	{
	}
	/**
		バーチャルイテレータrhsが内包するイテレータを内包するバーチャルイテレータを作成します。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_input_iterator(const virtual_forward_iterator<T, Ref, Ptr, Dist>& rhs) : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		rhs.delivered_abstract()
		)
	{
	}
#if defined(_MSC_VER)
	~virtual_input_iterator()
	{
	}
	virtual_input_iterator& operator=(const virtual_input_iterator& rhs)
	{
		_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >::operator=(rhs);
		return *this;
	}
#endif
	Ref operator*() const
	{
		return abstract()->reference();
	}
	typename _virtual_iterator_void_alternative<Ptr>::type operator->() const
	{
		return abstract()->pointer();
	}
	virtual_input_iterator& operator++()
	{
		own_abstract()->increment();
		return *this;
	}
	virtual_input_iterator operator++(int)
	{
		virtual_input_iterator old = *this;
		own_abstract()->increment();
		return old;
	}
	friend static inline bool operator==(const virtual_input_iterator& lhs, const virtual_input_iterator& rhs)
	{
		return lhs.abstract()->equal(*rhs.abstract());
	}
	friend static inline bool operator!=(const virtual_input_iterator& lhs, const virtual_input_iterator& rhs)
	{
		return !operator==(lhs, rhs);
	}
};

/**
	出力反復子を定義します。
	<p>このクラスは次の条件を満たすようなイテレータItを内包します。</p>
	<ul>
	<li>std::iterator_traits<It>::value_type 型が T型</li>
	<li>std::iterator_traits<It>::reference_type 型が void型</li>
	<li>std::iterator_traits<It>::pointer_type 型が void型</li>
	<li>std::iterator_traits<It>::distance_type 型が void型</li>
	</ul>
	<p>このクラスは、Tのみでパラメータ化され、Itによるパラメータ化はされませんので、この条件を満たすあらゆるItを、同じ型のイテレータとして内包できます。</p>
*/
template <class T>
class virtual_output_iterator : public _VITERATOR_STL_ITERATOR(std::output_iterator_tag, void, void, void, void), public _virtual_iterator_base<_virtual_output_iterator_abstract<T> >
{
public:
	/**
		未定義のイテレータを内包するバーチャルイテレータを作成します。
	*/
	virtual_output_iterator() : _virtual_iterator_base<_virtual_output_iterator_abstract<T> >(
		new _virtual_output_iterator_implements<T *, T>(0)
		)
	{
	}
	/**
		It型のイテレータrhsを内包するバーチャルイテレータを作成します。
		@param It
			イテレータの型。
		@param rhs
			イテレータのインスタンス。
	*/
	template <class It>
	virtual_output_iterator(const It& rhs) : _virtual_iterator_base<_virtual_output_iterator_abstract<T> >(
		new _virtual_output_iterator_implements<It, T>(rhs)
		)
	{
	}
	/**
		バーチャルイテレータrhsと同じイテレータを作成するコピーコンストラクタです。
		@param rhs
			バーチャルイテレータのインスタンス。
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_output_iterator(const virtual_output_iterator& rhs) : _virtual_iterator_base<_virtual_output_iterator_abstract<T> >(
		rhs
		)
	{
	}
#if defined(_MSC_VER)
	~virtual_output_iterator()
	{
	}
	virtual_output_iterator& operator=(const virtual_output_iterator& rhs)
	{
		_virtual_iterator_base<_virtual_output_iterator_abstract<T> >::operator=(rhs);
		return *this;
	}
#endif
	virtual_output_iterator operator*()
	{
		return *this;
	}
	virtual_output_iterator& operator++()
	{
		own_abstract()->increment();
		return *this;
	}
	virtual_output_iterator operator++(int)
	{
		virtual_output_iterator old = *this;
		own_abstract()->increment();
		return old;
	}
	virtual_output_iterator operator=(const T& rhs)
	{
		own_abstract()->output(rhs);
		return *this;
	}
};

#endif // !defined(VITERATOR_H_)
