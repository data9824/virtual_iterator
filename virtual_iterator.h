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
#pragma warning(disable:4786) // �f�o�b�O���؂�̂Ẵ��[�j���O
#pragma warning(disable:4284) // int *�Ȃǂ�Ԃ�operator->�̃R���p�C�����̃��[�j���O
#endif // _MSC_VER > 1000

#if defined(_MSC_VER) && _MSC_VER < 1400 && !defined(__SGI_STL) && !defined(_STLPORT_VERSION)
// Visual C++�t����STL�ɂ��킹��
#define _VITERATOR_STL_ITERATOR(tag, t, dist, ptr, ref) std::iterator<tag, t, dist>
#else
#define _VITERATOR_STL_ITERATOR(tag, t, dist, ptr, ref) std::iterator<tag, t, dist, ptr, ref>
#endif

#if defined(_MSC_VER) && _MSC_VER < 1400
// �ϊ��R���X�g���N�^���e���v���[�g�R���X�g���N�^�̓��ꉻ�ƒ�`���܂��B
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
	�o�[�`�����C�e���[�^�̎��ۂ̋@�\��񋟂���N���X�̊��N���X�ƂȂ�܂��B
	<p>�Q�ƃJ�E���g��ێ����A����ɂ��킹�Ď��g�������폜���܂��B</p>
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
	�o�[�`�����C�e���[�^���p�����ׂ��N���X�ł��B
	<p>_virtual_iterator_abstract�̎Q�ƃJ�E���g�⏊�L���̊Ǘ����s���܂��B</p>
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
		// �����ŃA�T�[�g�����s�����ꍇ�A��r�������ӂ̃o�[�`�����C�e���[�^�������
		// �C�e���[�^�̌^���قȂ�\��������܂��B
		// ���̂悤�Ȕ�r�ɑ΂��錋�ʂ͕s��i�������A�N�Z�X�ᔽ�̉\������j�ł��̂Œ��ӂ��Ă��������B
		assert(containted_iterator_id() == rhs.containted_iterator_id());
	}
#else
	void assert_containted_iterator_id(const _virtual_iterator_containted_iterator_id_checker& rhs) const
	{
	}
#endif // NDEBUG
};

/**
	virtual_input_iterator�̋@�\��񋟂��钊�ۃN���X�ł��B
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
	virtual_output_iterator�̋@�\��񋟂��钊�ۃN���X�ł��B
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
	virtual_forward_iterator�̋@�\��񋟂��钊�ۃN���X�ł��B
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
	virtual_bidirectional_iterator�̋@�\��񋟂��钊�ۃN���X�ł��B
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
	virtual_random_access_iterator�̋@�\��񋟂��钊�ۃN���X�ł��B
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
	virtual_input_iterator�̋@�\���AIt�^�̃C�e���[�^�ɂ��񋟂��܂��B
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
/*		// ��{�N���X�̃|�C���^�������o�[�`�����C�e���[�^���A�h���N���X�̃|�C���^������
		// �C�e���[�^�����Ă���Ƃ��ɁA*m_it�ŕԂ��ꂽ�h���N���X�̃|�C���^�ւ̎Q��
		// ���烍�[�J���I�u�W�F�N�g�Ƃ��āA�h���N���X�̃|�C���^���쐬����Ă��܂��A
		// ���[�J���I�u�W�F�N�g�̎Q�Ƃ�Ԃ��Ă��܂��̂�h�����߁Astatic_cast����B
		return static_cast<Ref>(*m_it);*/
		return *m_it;
	}
	virtual Ptr pointer() const
	{
/*		// ��{�N���X�̃|�C���^�������o�[�`�����C�e���[�^���A�h���N���X�̃|�C���^������
		// �C�e���[�^�����Ă���Ƃ��ɁA*m_it�ŕԂ��ꂽ�h���N���X�̃|�C���^�ւ̎Q��
		// �ɑ΂���&*m_it�Ƃ��邱�ƂŁA�h���N���X�̃|�C���^�ւ̃|�C���^�ƂȂ��Ă��܂�
		// �Ԃ�l�̌^�ł���Ptr�ɕϊ��ł��Ȃ��Ȃ�̂�h�����߁Astatic_cast����B
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
	virtual_input_iterator��operator->�������@�\���AIt�^�̃C�e���[�^�ɂ��񋟂��܂��B
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
	virtual_output_iterator�̋@�\���AIt�^�̃C�e���[�^�ɂ��񋟂��܂��B
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
	virtual_forward_iterator�̋@�\���AIt�^�̃C�e���[�^�ɂ��񋟂��܂��B
	<p>virtual_input_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_input_iterator�̋@�\�����s����܂��B</p>
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
	virtual_forward_iterator��operator->�������@�\���AIt�^�̃C�e���[�^�ɂ��񋟂��܂��B
	<p>virtual_input_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_input_iterator�̋@�\�����s����܂��B</p>
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
	virtual_bidirectional_iterator�̋@�\���AIt�^�̃C�e���[�^�ɂ��񋟂��܂��B
	<p>virtual_input_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_input_iterator�̋@�\�����s����܂��B</p>
	<p>virtual_forward_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_forward_iterator�̋@�\�����s����܂��B</p>
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
		// �����ŗႦ�΁Aconst int����int�ɕϊ��ł��Ȃ��Ƃ̃R���p�C���G���[���ł��ꍇ�́A
		// virtual_bidirectional_iterator<int>�̃R���X�g���N�^�ɁAstd::list<int>::const_iterator�Ȃǂ�
		// �萔�C�e���[�^��n���Ă���\��������܂��B
		// �萔�C�e���[�^���󂯎���̂́A�萔�o�[�`�����C�e���[�^�ł���
		// virtual_bidirectional_iterator<const int>�ł��B
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
	virtual_bidirectional_iterator��operator->�������@�\���AIt�^�̃C�e���[�^�ɂ��񋟂��܂��B
	<p>virtual_input_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_input_iterator�̋@�\�����s����܂��B</p>
	<p>virtual_forward_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_forward_iterator�̋@�\�����s����܂��B</p>
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
		// �����ŗႦ�΁Aconst int����int�ɕϊ��ł��Ȃ��Ƃ̃R���p�C���G���[���ł��ꍇ�́A
		// virtual_bidirectional_iterator<int>�̃R���X�g���N�^�ɁAstd::list<int>::const_iterator�Ȃǂ�
		// �萔�C�e���[�^��n���Ă���\��������܂��B
		// �萔�C�e���[�^���󂯎���̂́A�萔�o�[�`�����C�e���[�^�ł���
		// virtual_bidirectional_iterator<const int>�ł��B
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
	virtual_random_access_iterator�̋@�\���AIt�^�̃C�e���[�^�ɂ��񋟂��܂��B
	<p>virtual_input_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_input_iterator�̋@�\�����s����܂��B</p>
	<p>virtual_forward_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_forward_iterator�̋@�\�����s����܂��B</p>
	<p>virtual_bidirectional_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_bidirectional_iterator�̋@�\�����s����܂��B</p>
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
		// �����ŗႦ�΁Aconst int����int�ɕϊ��ł��Ȃ��Ƃ̃R���p�C���G���[���ł��ꍇ�́A
		// virtual_random_access_iterator<int>�̃R���X�g���N�^�ɁAstd::vector<int>::const_iterator�Ȃǂ�
		// �萔�C�e���[�^��n���Ă���\��������܂��B
		// �萔�C�e���[�^���󂯎���̂́A�萔�o�[�`�����C�e���[�^�ł���
		// virtual_random_access_iterator<const int>�ł��B
		return *m_it;
	}
	virtual Ptr pointer() const
	{
		return &*m_it;
	}
	virtual Ref brackets(Dist n) const
	{
		// �����ŗႦ�΁Aconst int����int�ɕϊ��ł��Ȃ��Ƃ̃R���p�C���G���[���ł��ꍇ�́A
		// virtual_random_access_iterator<int>�̃R���X�g���N�^�ɁAstd::vector<int>::const_iterator�Ȃǂ�
		// �萔�C�e���[�^��n���Ă���\��������܂��B
		// �萔�C�e���[�^���󂯎���̂́A�萔�o�[�`�����C�e���[�^�ł���
		// virtual_random_access_iterator<const int>�ł��B
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
	virtual_random_access_iterator��operator->�������@�\���AIt�^�̃C�e���[�^�ɂ��񋟂��܂��B
	<p>virtual_input_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_input_iterator�̋@�\�����s����܂��B</p>
	<p>virtual_forward_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_forward_iterator�̋@�\�����s����܂��B</p>
	<p>virtual_bidirectional_iterator�̋@�\���������Ă���A_virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >
	����Ăяo���ꂽ�ꍇ�ɂ́Avirtual_bidirectional_iterator�̋@�\�����s����܂��B</p>
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
		// �����ŗႦ�΁Aconst int����int�ɕϊ��ł��Ȃ��Ƃ̃R���p�C���G���[���ł��ꍇ�́A
		// virtual_random_access_iterator<int>�̃R���X�g���N�^�ɁAstd::vector<int>::const_iterator�Ȃǂ�
		// �萔�C�e���[�^��n���Ă���\��������܂��B
		// �萔�C�e���[�^���󂯎���̂́A�萔�o�[�`�����C�e���[�^�ł���
		// virtual_random_access_iterator<const int>�ł��B
		return *m_it;
	}
	virtual Ptr pointer() const
	{
	}
	virtual Ref brackets(Dist n) const
	{
		// �����ŗႦ�΁Aconst int����int�ɕϊ��ł��Ȃ��Ƃ̃R���p�C���G���[���ł��ꍇ�́A
		// virtual_random_access_iterator<int>�̃R���X�g���N�^�ɁAstd::vector<int>::const_iterator�Ȃǂ�
		// �萔�C�e���[�^��n���Ă���\��������܂��B
		// �萔�C�e���[�^���󂯎���̂́A�萔�o�[�`�����C�e���[�^�ł���
		// virtual_random_access_iterator<const int>�ł��B
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
	�����_���A�N�Z�X�����q���`���܂��B
	<p>���̃N���X�͎��̏����𖞂����悤�ȃC�e���[�^It�����܂��B</p>
	<ul>
	<li>std::iterator_traits<It>::value_type �^�� T�^</li>
	<li>std::iterator_traits<It>::reference_type �^�� Ref�^</li>
	<li>std::iterator_traits<It>::pointer_type �^�� Ptr�^</li>
	<li>std::iterator_traits<It>::distance_type �^�� Dist�^</li>
	</ul>
	<p>���̃N���X�́AT, Ref, Ptr, Dist�݂̂Ńp�����[�^������AIt�ɂ��p�����[�^���͂���܂���̂ŁA���̏����𖞂���������It���A�����^�̃C�e���[�^�Ƃ��ē���ł��܂��B</p>
	<p>�萔�C�e���[�^���쐬�������ꍇ�́A�e���v���[�g�p�����[�^T��const�^���w�肵�Ă��������B</p>
*/
template <class T, class Ref = T&, class Ptr = T *, class Dist = ptrdiff_t>
class virtual_random_access_iterator : public _VITERATOR_STL_ITERATOR(std::random_access_iterator_tag, T, Dist, Ptr, Ref), public _virtual_iterator_base<_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> >
{
public:
	/**
		����`�̃C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
	*/
	virtual_random_access_iterator() : _virtual_iterator_base<_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> >(
		new _virtual_random_access_iterator_factory<Ptr>::abstract<T *, T, Ref>::type(0)
		)
	{
	}
	/**
		It�^�̃C�e���[�^rhs������o�[�`�����C�e���[�^���쐬���܂��B
		@param It
			�C�e���[�^�̌^�B
		@param rhs
			�C�e���[�^�̃C���X�^���X�B
	*/
	template <class It>
	virtual_random_access_iterator(const It& rhs) : _virtual_iterator_base<_virtual_random_access_iterator_abstract<T, Ref, Ptr, Dist> >(
		new _virtual_random_access_iterator_factory<Ptr>::abstract<It, T, Ref>::type(rhs)
		)
	{
	}
	/**
		�o�[�`�����C�e���[�^rhs�Ɠ����C�e���[�^���쐬����R�s�[�R���X�g���N�^�ł��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
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
	�o���������q���`���܂��B
	<p>���̃N���X�͎��̏����𖞂����悤�ȃC�e���[�^It�����܂��B</p>
	<ul>
	<li>std::iterator_traits<It>::value_type �^�� T�^</li>
	<li>std::iterator_traits<It>::reference_type �^�� Ref�^</li>
	<li>std::iterator_traits<It>::pointer_type �^�� Ptr�^</li>
	<li>std::iterator_traits<It>::distance_type �^�� Dist�^</li>
	</ul>
	<p>���̃N���X�́AT, Ref, Ptr, Dist�݂̂Ńp�����[�^������AIt�ɂ��p�����[�^���͂���܂���̂ŁA���̏����𖞂���������It���A�����^�̃C�e���[�^�Ƃ��ē���ł��܂��B</p>
	<p>�萔�C�e���[�^���쐬�������ꍇ�́A�e���v���[�g�p�����[�^T��const�^���w�肵�Ă��������B</p>
*/
template <class T, class Ref = T&, class Ptr = T *, class Dist = ptrdiff_t>
class virtual_bidirectional_iterator : public _VITERATOR_STL_ITERATOR(std::bidirectional_iterator_tag, T, Dist, Ptr, Ref), public _virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >
{
public:
	/**
		����`�̃C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
	*/
	virtual_bidirectional_iterator() : _virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_bidirectional_iterator_factory<Ptr>::abstract<T *, T, Ref>::type(0)
		)
	{
	}
	/**
		It�^�̃C�e���[�^rhs������o�[�`�����C�e���[�^���쐬���܂��B
		@param It
			�C�e���[�^�̌^�B
		@param rhs
			�C�e���[�^�̃C���X�^���X�B
	*/
	template <class It>
	virtual_bidirectional_iterator(const It& rhs) : _virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_bidirectional_iterator_factory<Ptr>::abstract<It, T, Ref>::type(rhs)
		)
	{
	}
	/**
		�o�[�`�����C�e���[�^rhs�Ɠ����C�e���[�^���쐬����R�s�[�R���X�g���N�^�ł��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_bidirectional_iterator(const virtual_bidirectional_iterator& rhs) : _virtual_iterator_base<_virtual_bidirectional_iterator_abstract<T, Ref, Ptr> >(
		rhs
		)
	{
	}
	// ���@�\�ȃJ�e�S���̃o�[�`�����C�e���[�^�Œ�@�\�ȃJ�e�S���̃o�[�`�����C�e���[�^�ւ�
	// �ϊ������o�֐����`���Ȃ��ŁA��@�\���̕ϊ��R���X�g���N�^�����@�\���̃o�[�`�����C�e���[�^��
	// �����ɂƂ�悤�ɂ��Ă���̂́A��@�\���̃o�[�`�����C�e���[�^�������ɂƂ�悤�Ȋ֐���
	// ���@�\���̃o�[�`�����C�e���[�^��n���Ƃ��A��@�\���̖��ʂȃR�s�[�𔭐������Ȃ����߂ł���
	/**
		�o�[�`�����C�e���[�^rhs�������C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
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
	�O�������q���`���܂��B
	<p>���̃N���X�͎��̏����𖞂����悤�ȃC�e���[�^It�����܂��B</p>
	<ul>
	<li>std::iterator_traits<It>::value_type �^�� T�^</li>
	<li>std::iterator_traits<It>::reference_type �^�� Ref�^</li>
	<li>std::iterator_traits<It>::pointer_type �^�� Ptr�^</li>
	<li>std::iterator_traits<It>::distance_type �^�� Dist�^</li>
	</ul>
	<p>���̃N���X�́AT, Ref, Ptr, Dist�݂̂Ńp�����[�^������AIt�ɂ��p�����[�^���͂���܂���̂ŁA���̏����𖞂���������It���A�����^�̃C�e���[�^�Ƃ��ē���ł��܂��B</p>
	<p>�萔�C�e���[�^���쐬�������ꍇ�́Avirtual_input_iterator<T, Ref, Ptr, Dist>�𗘗p���Ă��������B</p>
*/
template <class T, class Ref = T&, class Ptr = T *, class Dist = ptrdiff_t>
class virtual_forward_iterator : public _VITERATOR_STL_ITERATOR(std::forward_iterator_tag, T, Dist, Ptr, Ref), public _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >
{
public:
	/**
		����`�̃C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
	*/
	virtual_forward_iterator() : _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_forward_iterator_factory<Ptr>::abstract<T *, T, Ref>::type(0)
		)
	{
	}
	/**
		It�^�̃C�e���[�^rhs������o�[�`�����C�e���[�^���쐬���܂��B
		@param It
			�C�e���[�^�̌^�B
		@param rhs
			�C�e���[�^�̃C���X�^���X�B
	*/
	template <class It>
	virtual_forward_iterator(const It& rhs) : _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_forward_iterator_factory<Ptr>::abstract<It, T, Ref>::type(rhs)
		)
	{
	}
	/**
		�o�[�`�����C�e���[�^rhs�Ɠ����C�e���[�^���쐬����R�s�[�R���X�g���N�^�ł��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_forward_iterator(const virtual_forward_iterator& rhs) : _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >(
		rhs
		)
	{
	}
	// ���@�\�ȃJ�e�S���̃o�[�`�����C�e���[�^�Œ�@�\�ȃJ�e�S���̃o�[�`�����C�e���[�^�ւ�
	// �ϊ������o�֐����`���Ȃ��ŁA��@�\���̕ϊ��R���X�g���N�^�����@�\���̃o�[�`�����C�e���[�^��
	// �����ɂƂ�悤�ɂ��Ă���̂́A��@�\���̃o�[�`�����C�e���[�^�������ɂƂ�悤�Ȋ֐���
	// ���@�\���̃o�[�`�����C�e���[�^��n���Ƃ��A��@�\���̖��ʂȃR�s�[�𔭐������Ȃ����߂ł���
	/**
		�o�[�`�����C�e���[�^rhs�������C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_forward_iterator(const virtual_random_access_iterator<T, Ref, Ptr, Dist>& rhs) : _virtual_iterator_base<_virtual_forward_iterator_abstract<T, Ref, Ptr> >(
		rhs.delivered_abstract()
		)
	{
	}
	/**
		�o�[�`�����C�e���[�^rhs�������C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
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
	���͔����q���`���܂��B
	<p>���̃N���X�͎��̏����𖞂����悤�ȃC�e���[�^It�����܂��B</p>
	<ul>
	<li>std::iterator_traits<It>::value_type �^�� T�^</li>
	<li>std::iterator_traits<It>::reference_type �^�� Ref�^</li>
	<li>std::iterator_traits<It>::pointer_type �^�� Ptr�^</li>
	<li>std::iterator_traits<It>::distance_type �^�� Dist�^</li>
	</ul>
	<p>���̃N���X�́AT, Ref, Ptr, Dist�݂̂Ńp�����[�^������AIt�ɂ��p�����[�^���͂���܂���̂ŁA���̏����𖞂���������It���A�����^�̃C�e���[�^�Ƃ��ē���ł��܂��B</p>
	<p>���̃N���X�͏�ɒ萔�C�e���[�^�ƂȂ�܂��B</p>
*/
template <class T, class Ref = const T&, class Ptr = const T *, class Dist = ptrdiff_t>
class virtual_input_iterator : public _VITERATOR_STL_ITERATOR(std::input_iterator_tag, T, Dist, Ptr, Ref), public _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >
{
public:
	/**
		����`�̃C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
	*/
	virtual_input_iterator() : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_input_iterator_factory<Ptr>::abstract<T *, T, Ref>::type(0)
		)
	{
	}
	/**
		It�^�̃C�e���[�^rhs������o�[�`�����C�e���[�^���쐬���܂��B
		@param It
			�C�e���[�^�̌^�B
		@param rhs
			�C�e���[�^�̃C���X�^���X�B
	*/
	template <class It>
	virtual_input_iterator(const It& rhs) : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		new _virtual_input_iterator_factory<Ptr>::abstract<It, T, Ref>::type(rhs)
		)
	{
	}
	/**
		�o�[�`�����C�e���[�^rhs�Ɠ����C�e���[�^���쐬����R�s�[�R���X�g���N�^�ł��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_input_iterator(const virtual_input_iterator& rhs) : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		rhs
		)
	{
	}
	// ���@�\�ȃJ�e�S���̃o�[�`�����C�e���[�^�Œ�@�\�ȃJ�e�S���̃o�[�`�����C�e���[�^�ւ�
	// �ϊ������o�֐����`���Ȃ��ŁA��@�\���̕ϊ��R���X�g���N�^�����@�\���̃o�[�`�����C�e���[�^��
	// �����ɂƂ�悤�ɂ��Ă���̂́A��@�\���̃o�[�`�����C�e���[�^�������ɂƂ�悤�Ȋ֐���
	// ���@�\���̃o�[�`�����C�e���[�^��n���Ƃ��A��@�\���̖��ʂȃR�s�[�𔭐������Ȃ����߂ł���
	/**
		�o�[�`�����C�e���[�^rhs�������C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_input_iterator(const virtual_random_access_iterator<T, Ref, Ptr, Dist>& rhs) : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		rhs.delivered_abstract()
		)
	{
	}
	/**
		�o�[�`�����C�e���[�^rhs�������C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
	*/
	_VITERATOR_CONVERTION_CONSTRUCTOR
	virtual_input_iterator(const virtual_bidirectional_iterator<T, Ref, Ptr, Dist>& rhs) : _virtual_iterator_base<_virtual_input_iterator_abstract<T, Ref, Ptr> >(
		rhs.delivered_abstract()
		)
	{
	}
	/**
		�o�[�`�����C�e���[�^rhs�������C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
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
	�o�͔����q���`���܂��B
	<p>���̃N���X�͎��̏����𖞂����悤�ȃC�e���[�^It�����܂��B</p>
	<ul>
	<li>std::iterator_traits<It>::value_type �^�� T�^</li>
	<li>std::iterator_traits<It>::reference_type �^�� void�^</li>
	<li>std::iterator_traits<It>::pointer_type �^�� void�^</li>
	<li>std::iterator_traits<It>::distance_type �^�� void�^</li>
	</ul>
	<p>���̃N���X�́AT�݂̂Ńp�����[�^������AIt�ɂ��p�����[�^���͂���܂���̂ŁA���̏����𖞂���������It���A�����^�̃C�e���[�^�Ƃ��ē���ł��܂��B</p>
*/
template <class T>
class virtual_output_iterator : public _VITERATOR_STL_ITERATOR(std::output_iterator_tag, void, void, void, void), public _virtual_iterator_base<_virtual_output_iterator_abstract<T> >
{
public:
	/**
		����`�̃C�e���[�^������o�[�`�����C�e���[�^���쐬���܂��B
	*/
	virtual_output_iterator() : _virtual_iterator_base<_virtual_output_iterator_abstract<T> >(
		new _virtual_output_iterator_implements<T *, T>(0)
		)
	{
	}
	/**
		It�^�̃C�e���[�^rhs������o�[�`�����C�e���[�^���쐬���܂��B
		@param It
			�C�e���[�^�̌^�B
		@param rhs
			�C�e���[�^�̃C���X�^���X�B
	*/
	template <class It>
	virtual_output_iterator(const It& rhs) : _virtual_iterator_base<_virtual_output_iterator_abstract<T> >(
		new _virtual_output_iterator_implements<It, T>(rhs)
		)
	{
	}
	/**
		�o�[�`�����C�e���[�^rhs�Ɠ����C�e���[�^���쐬����R�s�[�R���X�g���N�^�ł��B
		@param rhs
			�o�[�`�����C�e���[�^�̃C���X�^���X�B
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
