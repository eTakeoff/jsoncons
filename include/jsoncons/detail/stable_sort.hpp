#ifndef JSONCONS_DETAIL_STABLE_SORT_HPP
#define JSONCONS_DETAIL_STABLE_SORT_HPP

#include <utility>
#include <limits>
#include <type_traits>
#include <xmemory>

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,_STL_WARNING_LEVEL)
#pragma warning(disable: _STL_DISABLED_WARNINGS)
#pragma push_macro("new")
#undef new

namespace jsoncons { 
namespace detail {

// COMMON SORT PARAMETERS
const int _ISORT_MAX = 32;	// maximum size for insertion sort


		// STRUCT TEMPLATE _Temporary_buffer
template<class _Ty>
	struct _Temporary_buffer
	{	// temporary storage
	explicit _Temporary_buffer(const ptrdiff_t _Requested_size)
		{	// get temporary storage
		const std::pair<_Ty *, ptrdiff_t> _Raw = std::_Get_temporary_buffer<_Ty>(_Requested_size);
		_Data = _Raw.first;
		_Capacity = _Raw.second;
		}

	_Temporary_buffer(const _Temporary_buffer&) = delete;
	_Temporary_buffer& operator=(const _Temporary_buffer&) = delete;

	~_Temporary_buffer() noexcept
		{	// return temporary storage
		_Return_temporary_buffer(_Data);
		}

	_Ty * _Data;
	ptrdiff_t _Capacity;
	};


		// STRUCT TEMPLATE _Temporary_range
template<class _Ty>
	struct _Temporary_range
	{	// a range of objects constructed in a temporary buffer
	using value_type = _Ty;

	explicit _Temporary_range(_Temporary_buffer<_Ty>& _Buffer)
		: _Data(_Buffer._Data),
		_Capacity(_Buffer._Capacity),
		_Size(0)
		{	// construct a range around a temporary buffer
		}

	template<class _FwdIt>
		_Temporary_range(_Temporary_buffer<_Ty>& _Buffer,
			const _FwdIt _First, const _FwdIt _Last, const std::_Iter_diff_t<_FwdIt> _Count)
		: _Data(_Buffer._Data),
		_Capacity(_Buffer._Capacity),
		_Size(_Count)
		{	// construct a range around a temporary buffer, and move another range into it
		_Uninitialized_move_unchecked(_First, _Last, _Data);
		}

	_Temporary_range(const _Temporary_range&) = delete;
	_Temporary_range& operator=(const _Temporary_range&) = delete;

	~_Temporary_range() noexcept
		{	// destroy constructed elements
		_Destroy_range(_Data, _Data + _Size);
		}

	_Ty * _Begin()
		{	// get the beginning of this range
		return (_Data);
		}

	_Ty * _End()
		{	// get the end of this range
		return (_Data + _Size);
		}

	void push_back(_Ty&& _Val)
		{	// add an element to the end
		_Construct_in_place(_Data[_Size], _STD move(_Val));
		++_Size;
		}

	_Ty * _Data;
	ptrdiff_t _Capacity;
	ptrdiff_t _Size;
	};

		// FUNCTION TEMPLATE for_each
template<class _InIt,
	class _Fn> inline
	_Fn for_each(_InIt _First, _InIt _Last, _Fn _Func)
	{	// perform function for each element [_First, _Last)
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		_Func(*_UFirst);
		}

	return (_Func);
	}

#if _HAS_CXX17
template<class _ExPo,
	class _FwdIt,
	class _Fn,
	_Enable_if_execution_policy_t<_ExPo> = 0> inline
	void for_each(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Fn _Func) noexcept;

		// FUNCTION TEMPLATE for_each_n
template<class _InIt,
	class _Diff,
	class _Fn> inline
	_InIt for_each_n(_InIt _First, const _Diff _Count_raw, _Fn _Func)
	{	// perform function for each element [_First, _First + _Count)
	std::_Algorithm_int_t<_Diff> _Count = _Count_raw;
	auto _UFirst = std::_Unchecked_n(_First, _Count);
	for (; 0 < _Count; --_Count, (void)++_UFirst)
		{
		_Func(*_UFirst);
		}

	return (_Rechecked(_First, _UFirst));
	}

template<class _ExPo,
	class _FwdIt,
	class _Diff,
	class _Fn,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	_FwdIt for_each_n(_ExPo&& _Exec, _FwdIt _First, _Diff _Count_raw, _Fn _Func) noexcept;

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InTy,
	size_t _InSize,
	class _Diff,
	class _Fn> inline
	_InTy * for_each_n(_InTy (&_First)[_InSize], const _Diff _Count_raw, _Fn _Func)
	{	// perform function for each element [_First, _First + _Count)
	std::_Algorithm_int_t<_Diff> _Count = _Count_raw;
	_DEBUG_ARRAY_SIZE(_First, _Count);
	_InTy * _UFirst = _First;
	for (; 0 < _Count; --_Count, (void)++_UFirst)
		{
		_Func(*_UFirst);
		}

	return (_UFirst);
	}

template<class _ExPo,
	class _SourceTy,
	size_t _SourceSize,
	class _Diff,
	class _Fn,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	_SourceTy * for_each_n(_ExPo&& _Exec, _SourceTy (&_First)[_SourceSize], _Diff _Count_raw, _Fn _Func) noexcept;
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */
#endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE find_if
template<class _InIt,
	class _Pr> inline
	_InIt _Find_if_unchecked(_InIt _First, _InIt _Last, _Pr _Pred)
	{	// find first satisfying _Pred
	for (; _First != _Last; ++_First)
		{
		if (_Pred(*_First))
			{
			break;
			}
		}

	return (_First);
	}

template<class _InIt,
	class _Pr> inline
	_InIt find_if(_InIt _First, _InIt _Last, _Pr _Pred)
	{	// find first satisfying _Pred
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Find_if_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred))));
	}

		// FUNCTION TEMPLATE adjacent_find WITH PRED
template<class _FwdIt,
	class _Pr> inline
	_FwdIt adjacent_find(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find first satisfying _Pred with successor
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	if (_UFirst != _ULast)
		{
		for (auto _UFirstb = _UFirst; ++_UFirst != _ULast; _UFirstb = _UFirst)
			{
			if (_Pred(*_UFirstb, *_UFirst))
				{
				return (_Rechecked(_Last, _UFirstb));
				}
			}
		}

	return (_Rechecked(_Last, _ULast));
	}

		// FUNCTION TEMPLATE adjacent_find
template<class _FwdIt> inline
	_FwdIt adjacent_find(_FwdIt _First, _FwdIt _Last)
	{	// find first matching successor
	return (adjacent_find(_First, _Last, std::equal_to<>()));
	}

		// FUNCTION TEMPLATE count_if
template<class _InIt,
	class _Pr> inline
	std::_Iter_diff_t<_InIt> count_if(_InIt _First, _InIt _Last, _Pr _Pred)
	{	// count elements satisfying _Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	std::_Iter_diff_t<_InIt> _Count = 0;
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (_Pred(*_UFirst))
			{
			++_Count;
			}
		}

	return (_Count);
	}

		// FUNCTION TEMPLATE mismatch WITH PRED
template<class _InIt1,
	class _InIt2,
	class _Pr> inline
	std::pair<_InIt1, _InIt2> _Mismatch_no_deprecate(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _Pr _Pred)
	{	// return [_First1, _Last1)/[_First2, ...) mismatch using _Pred, no deprecation warnings
	_DEBUG_RANGE(_First1, _Last1);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = std::_Unchecked_n(_First2, std::_Idl_distance<_InIt1>(_UFirst1, _ULast1));
	while (_UFirst1 != _ULast1 && _Pred(*_UFirst1, *_UFirst2))
		{	// point past match
		++_UFirst1;
		++_UFirst2;
		}

	return {_Rechecked(_First1, _UFirst1), _Rechecked(_First2, _UFirst2)};
	}

template<class _InIt1,
	class _InIt2,
	class _Pr> inline
	std::pair<_InIt1, _InIt2> mismatch(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _Pr _Pred)
	{	// return [_First1, _Last1)/[_First2, ...) mismatch using _Pred
	_DEPRECATE_UNCHECKED(mismatch, _First2);
	return (_Mismatch_no_deprecate(_First1, _Last1, _First2, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _RightTy,
	size_t _RightSize,
	class _Pr,
	class = enable_if_t<!is_same_v<_RightTy *, _Pr>>> inline
	std::pair<_InIt1, _RightTy *> mismatch(_InIt1 _First1, _InIt1 _Last1,
		_RightTy (&_First2)[_RightSize], _Pr _Pred)
	{	// return [_First1, _Last1)/[_First2, ...) mismatch using _Pred
	return (std::_Unchecked_second(_Mismatch_no_deprecate(_First1, _Last1,
		_Array_iterator<_RightTy, _RightSize>(_First2), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE mismatch
template<class _InIt1,
	class _InIt2> inline
	std::pair<_InIt1, _InIt2> mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	// return [_First1, _Last1)/[_First2, ...) mismatch
	return (mismatch(_First1, _Last1, _First2, std::equal_to<>()));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _RightTy,
	size_t _RightSize> inline
	std::pair<_InIt1, _RightTy *> mismatch(_InIt1 _First1, _InIt1 _Last1, _RightTy (&_First2)[_RightSize])
	{	// return [_First1, _Last1)/[_First2, ...) mismatch, array source
	return (_STD mismatch(_First1, _Last1, _First2, std::equal_to<>()));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE mismatch WITH TWO RANGES, PRED
template<class _InIt1,
	class _InIt2,
	class _Pr> inline
	std::pair<_InIt1, _InIt2> mismatch(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _Pr _Pred)
	{	// return [_First1, _Last1)/[_First2, _Last2) mismatch using _Pred
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	while (_UFirst1 != _ULast1 && _UFirst2 != _ULast2 && _Pred(*_UFirst1, *_UFirst2))
		{	// point past match
		++_UFirst1;
		++_UFirst2;
		}

	return {_Rechecked(_First1, _UFirst1), _Rechecked(_First2, _UFirst2)};
	}

		// FUNCTION TEMPLATE mismatch WITH TWO RANGES
template<class _InIt1,
	class _InIt2> inline
	std::pair<_InIt1, _InIt2> mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2)
	{	// return [_First1, _Last1)/[_First2, _Last2) mismatch
	return (mismatch(_First1, _Last1, _First2, _Last2, std::equal_to<>()));
	}

		// FUNCTION TEMPLATE all_of
template<class _InIt,
	class _Pr> inline
	bool all_of(_InIt _First, _InIt _Last, _Pr _Pred)
	{	// test if all elements satisfy _Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (!_Pred(*_UFirst))
			{
			return (false);
			}
		}

	return (true);
	}

#if _HAS_CXX17
template<class _ExPo,
	class _FwdIt,
	class _Pr,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	bool all_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept;
#endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE any_of
template<class _InIt,
	class _Pr> inline
	bool any_of(_InIt _First, _InIt _Last, _Pr _Pred)
	{	// test if any element satisfies _Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (_Pred(*_UFirst))
			{
			return (true);
			}
		}

	return (false);
	}

#if _HAS_CXX17
template<class _ExPo,
	class _FwdIt,
	class _Pr,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	bool any_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept;
#endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE none_of
template<class _InIt,
	class _Pr> inline
	bool none_of(_InIt _First, _InIt _Last, _Pr _Pred)
	{	// test if no elements satisfy _Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (_Pred(*_UFirst))
			{
			return (false);
			}
		}

	return (true);
	}

#if _HAS_CXX17
template<class _ExPo,
	class _FwdIt,
	class _Pr,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	bool none_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept;
#endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE find_if_not
template<class _InIt,
	class _Pr> inline
	_InIt find_if_not(_InIt _First, _InIt _Last, _Pr _Pred)
	{	// find first element that satisfies !_Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (!_Pred(*_UFirst))
			{
			break;
			}
		}

	return (_Rechecked(_First, _UFirst));
	}

		// FUNCTION TEMPLATE copy_if
template<class _InIt,
	class _OutIt,
	class _Pr> inline
	_OutIt _Copy_if_no_deprecate(_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred)
	{	// copy each satisfying _Pred, no deprecation warnings
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest = std::_Unchecked_idl0(_Dest);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (_Pred(*_UFirst))
			{
			*_UDest = *_UFirst;
			++_UDest;
			}
		}

	return (_Rechecked(_Dest, _UDest));
	}

template<class _InIt,
	class _OutIt,
	class _Pr> inline
	_OutIt copy_if(_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred)
	{	// copy each satisfying _Pred
	_DEPRECATE_UNCHECKED(copy_if, _Dest);
	return (_Copy_if_no_deprecate(_First, _Last, _Dest, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt,
	class _DestTy,
	size_t _DestSize,
	class _Pr> inline
	_DestTy *copy_if(_InIt _First, _InIt _Last, _DestTy (&_Dest)[_DestSize],
		_Pr _Pred)
	{	// copy each satisfying _Pred, array dest
	return (_Unchecked(
		_Copy_if_no_deprecate(_First, _Last,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE partition_copy
template<class _InIt,
	class _OutIt1,
	class _OutIt2,
	class _Pr> inline
	std::pair<_OutIt1, _OutIt2>
		_Partition_copy_no_deprecate(_InIt _First, _InIt _Last,
			_OutIt1 _Dest1, _OutIt2 _Dest2, _Pr _Pred)
	{	// copy true partition to _Dest1, false to _Dest2, no deprecation warnings
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest1 = std::_Unchecked_idl0(_Dest1);
	auto _UDest2 = std::_Unchecked_idl0(_Dest2);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (_Pred(*_UFirst))
			{
			*_UDest1 = *_UFirst;
			++_UDest1;
			}
		else
			{
			*_UDest2 = *_UFirst;
			++_UDest2;
			}
		}

	return {_Rechecked(_Dest1, _UDest1), _Rechecked(_Dest2, _UDest2)};
	}

template<class _InIt,
	class _OutIt1,
	class _OutIt2,
	class _Pr> inline
	std::pair<_OutIt1, _OutIt2>
		partition_copy(_InIt _First, _InIt _Last,
			_OutIt1 _Dest1, _OutIt2 _Dest2, _Pr _Pred)
	{	// copy true partition to _Dest1, false to _Dest2
	return (_Partition_copy_no_deprecate(_First, _Last, _Dest1, _Dest2, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt,
	class _DestTy1,
	size_t _DestSize1,
	class _OutIt2,
	class _Pr> inline
	std::pair<_DestTy1 *, _OutIt2>
		partition_copy(_InIt _First, _InIt _Last,
			_DestTy1 (&_Dest1)[_DestSize1], _OutIt2 _Dest2, _Pr _Pred)
	{	// copy true partition to _Dest1, false to _Dest2, array dest
	_DEPRECATE_UNCHECKED(partition_copy, _Dest2);
	return (std::_Unchecked_first(
		_Partition_copy_no_deprecate(_First, _Last,
			_Array_iterator<_DestTy1, _DestSize1>(_Dest1), _Dest2, _Pass_fn(_Pred))));
	}

template<class _InIt,
	class _OutIt1,
	class _DestTy2,
	size_t _DestSize2,
	class _Pr> inline
	std::pair<_OutIt1, _DestTy2 *>
		partition_copy(_InIt _First, _InIt _Last,
			_OutIt1 _Dest1, _DestTy2 (&_Dest2)[_DestSize2], _Pr _Pred)
	{	// copy true partition to _Dest1, false to _Dest2, array dest
	_DEPRECATE_UNCHECKED(partition_copy, _Dest1);
	return (std::_Unchecked_second(
		_Partition_copy_no_deprecate(_First, _Last,
			_Dest1, _Array_iterator<_DestTy2, _DestSize2>(_Dest2), _Pass_fn(_Pred))));
	}

template<class _InIt,
	class _DestTy1,
	size_t _DestSize1,
	class _DestTy2,
	size_t _DestSize2,
	class _Pr> inline
	std::pair<_DestTy1 *, _DestTy2 *>
		partition_copy(_InIt _First, _InIt _Last,
			_DestTy1 (&_Dest1)[_DestSize1], _DestTy2 (&_Dest2)[_DestSize2],
			_Pr _Pred)
	{	// copy true partition to _Dest1, false to _Dest2, array dest
	return (std::_Unchecked_both(
		_Partition_copy_no_deprecate(_First, _Last,
				_Array_iterator<_DestTy1, _DestSize1>(_Dest1),
				_Array_iterator<_DestTy2, _DestSize2>(_Dest2), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE is_partitioned
template<class _InIt,
	class _Pr> inline
	bool is_partitioned(_InIt _First, _InIt _Last, _Pr _Pred)
	{	// test if [_First, _Last) partitioned by _Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);

	for (;; ++_UFirst)
		{	// skip true partition
		if (_UFirst == _ULast)
			{
			return (true);
			}

		if (!_Pred(*_UFirst))
			{
			break;
			}
		}

	while (++_UFirst != _ULast)
		{	// verify false partition
		if (_Pred(*_UFirst))
			{
			return (false);	// found out of place element
			}
		}

	return (true);
	}

		// FUNCTION TEMPLATE partition_point
template<class _FwdIt,
	class _Pr> inline
	_FwdIt partition_point(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find beginning of false partition in [_First, _Last)
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _Count = _STD distance(_UFirst, _ULast);
	while (0 < _Count)
		{	// divide and conquer, find half that contains answer
		const auto _Count2 = _Count / static_cast<decltype(_Count)>(2);
		const auto _UMid = _STD next(_UFirst, _Count2);

		if (_Pred(*_UMid))
			{	// try top half
			_UFirst = _Next_iter(_UMid);
			_Count -= _Count2 + 1;
			}
		else
			{
			_Count = _Count2;
			}
		}

	return (_Rechecked(_First, _UFirst));
	}

		// FUNCTION TEMPLATE search WITH PRED
template<class _FwdItHaystack,
	class _FwdItPat,
	class _Pr> inline
	_FwdItHaystack _Search_unchecked(_FwdItHaystack _First1, _FwdItHaystack _Last1,
		_FwdItPat _First2, _FwdItPat _Last2, _Pr _Pred,
		std::forward_iterator_tag, std::forward_iterator_tag)
	{	// find first [_First2, _Last2) satisfying _Pred, arbitrary iterators
	for (; ; ++_First1)
		{	// loop until match or end of a sequence
		_FwdItHaystack _Mid1 = _First1;
		for (_FwdItPat _Mid2 = _First2; ; ++_Mid1, (void)++_Mid2)
			{
			if (_Mid2 == _Last2)
				{
				return (_First1);
				}
			else if (_Mid1 == _Last1)
				{
				return (_Last1);
				}
			else if (!_Pred(*_Mid1, *_Mid2))
				{
				break;
				}
			}
		}
	}

template<class _FwdItHaystack,
	class _FwdItPat,
	class _Pr> inline
	_FwdItHaystack _Search_unchecked(_FwdItHaystack _First1, _FwdItHaystack _Last1,
		_FwdItPat _First2, _FwdItPat _Last2, _Pr _Pred,
		std::random_access_iterator_tag, std::random_access_iterator_tag)
	{	// find first [_First2, _Last2) satisfying _Pred, random-access iterators
	std::_Iter_diff_t<_FwdItHaystack> _Count1 = _Last1 - _First1;
	std::_Iter_diff_t<_FwdItPat> _Count2 = _Last2 - _First2;

	for (; _Count2 <= _Count1; ++_First1, (void)--_Count1)
		{	// room for match, try it
		_FwdItHaystack _Mid1 = _First1;
		for (_FwdItPat _Mid2 = _First2; ; ++_Mid1, (void)++_Mid2)
			{
			if (_Mid2 == _Last2)
				{
				return (_First1);
				}
			else if (!_Pred(*_Mid1, *_Mid2))
				{
				break;
				}
			}
		}

	return (_Last1);
	}

template<class _FwdItHaystack,
	class _FwdItPat,
	class _Pr> inline
	_FwdItHaystack search(_FwdItHaystack _First1, _FwdItHaystack _Last1,
		_FwdItPat _First2, _FwdItPat _Last2, _Pr _Pred)
	{	// find first [_First2, _Last2) satisfying _Pred
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	return (_Rechecked(_First1,
		_Search_unchecked(_Unchecked(_First1), _Unchecked(_Last1),
			_Unchecked(_First2), _Unchecked(_Last2), _Pass_fn(_Pred),
			std::_Iter_cat_t<_FwdItHaystack>(), std::_Iter_cat_t<_FwdItPat>())));
	}


		// FUNCTION TEMPLATE search WITH SEARCHER
template<class _FwdItHaystack,
	class _Searcher> inline
	_FwdItHaystack search(_FwdItHaystack _First, _FwdItHaystack _Last, const _Searcher& _Search)
	{	// find _Search's pattern in [_First, _Last)
	return (_Search(_First, _Last).first);
	}


		// FUNCTION TEMPLATE search
template<class _FwdItHaystack,
	class _FwdItPat> inline
	_FwdItHaystack search(_FwdItHaystack _First1, _FwdItHaystack _Last1,
		_FwdItPat _First2, _FwdItPat _Last2)
	{	// find first [_First2, _Last2) match
	return (search(_First1, _Last1, _First2, _Last2, std::equal_to<>()));
	}

		// FUNCTION TEMPLATE search_n WITH PRED
template<class _FwdIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	_FwdIt _Search_n_unchecked(_FwdIt _First, _FwdIt _Last,
		_Diff _Count, const _Ty& _Val, _Pr _Pred, std::forward_iterator_tag)
	{	// find first _Count * _Val satisfying _Pred, forward iterators
	if (_Count <= 0)
		return (_First);

	for (; _First != _Last; ++_First)
		{
		if (_Pred(*_First, _Val))
			{	// found start of possible match, check it out
			_FwdIt _Mid = _First;

			for (_Diff _Count1 = _Count;;)
				{
				if (--_Count1 == 0)
					{
					return (_First);	// found rest of match, report it
					}
				else if (++_Mid == _Last)
					{
					return (_Last);	// short match at end
					}
				else if (!_Pred(*_Mid, _Val))
					{	// short match not at end
					break;
					}
				}

			_First = _Mid;	// pick up just beyond failed match
			}
		}

	return (_Last);
	}

template<class _FwdIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	_FwdIt _Search_n_unchecked(_FwdIt _First, _FwdIt _Last,
		const _Diff _Count, const _Ty& _Val, _Pr _Pred, std::random_access_iterator_tag)
	{	// find first _Count * _Val satisfying _Pred, random-access iterators
	if (_Count <= 0)
		{
		return (_First);
		}

	if (static_cast<uintmax_t>(_Count)
		> static_cast<uintmax_t>((std::numeric_limits<std::_Iter_diff_t<_FwdIt>>::max)()))
		{	// if the number of _Vals searched for is larger than the longest possible
			// sequence, we can't find it
		return (_Last);
		}

	const auto _Count_diff = static_cast<std::_Iter_diff_t<_FwdIt>>(_Count);
	_FwdIt _Old_first = _First;
	for (std::_Iter_diff_t<_FwdIt> _Inc = 0; _Count_diff <= _Last - _Old_first; )
		{	// enough room, look for a match
		_First = _Old_first + _Inc;
		if (_Pred(*_First, _Val))
			{	// found part of possible match, check it out
			std::_Iter_diff_t<_FwdIt> _Count1 = _Count_diff;
			_FwdIt _Mid = _First;

			for (; _Old_first != _First && _Pred(_First[-1], _Val);
				--_First)
				--_Count1;	// back up over any skipped prefix

			if (_Count1 <= _Last - _Mid)
				{
				for (;;)
					{	// enough left, test suffix
					if (--_Count1 == 0)
						{
						return (_First);	// found rest of match, report it
						}
					else if (!_Pred(*++_Mid, _Val))
						{	// short match not at end
						break;
						}
					}
				}
			_Old_first = ++_Mid;	// failed match, take small jump
			_Inc = 0;
			}
		else
			{	// no match, take big jump and back up as needed
			_Old_first = _First + 1;
			_Inc = _Count_diff - 1;
			}
		}

	return (_Last);
	}

template<class _FwdIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	_FwdIt search_n(_FwdIt _First, _FwdIt _Last, const _Diff _Count_raw, const _Ty& _Val, _Pr _Pred)
	{	// find first _Count * _Val satisfying _Pred
	const std::_Algorithm_int_t<_Diff> _Count = _Count_raw;
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Search_n_unchecked(_Unchecked(_First), _Unchecked(_Last), _Count, _Val,
			_Pass_fn(_Pred), std::_Iter_cat_t<_FwdIt>())));
	}

		// FUNCTION TEMPLATE search_n
template<class _FwdIt,
	class _Diff,
	class _Ty> inline
	_FwdIt search_n(_FwdIt _First, _FwdIt _Last, const _Diff _Count, const _Ty& _Val)
	{	// find first _Count * _Val match
	return (search_n(_First, _Last, _Count, _Val, std::equal_to<>()));
	}

		// FUNCTION TEMPLATE find_end WITH PRED
template<class _FwdIt1,
	class _FwdIt2,
	class _Pr> inline
	_FwdIt1 find_end(_FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred)
	{	// find last [_First2, _Last2) satisfying _Pred
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	auto _Count1 = _STD distance(_UFirst1, _ULast1);
	const auto _Count2 = _STD distance(_UFirst2, _ULast2);
	auto _UAns = _ULast1;

	if (0 < _Count2)
		{
		for (; _Count2 <= _Count1; ++_UFirst1, (void)--_Count1)
			{	// room for match, try it
			auto _UMid1 = _UFirst1;
			for (auto _UMid2 = _UFirst2; ; ++_UMid1)
				{
				if (!_Pred(*_UMid1, *_UMid2))
					{
					break;
					}
				else if (++_UMid2 == _ULast2)
					{	// potential answer, save it
					_UAns = _UFirst1;
					break;
					}
				}
			}
		}

	return (_Rechecked(_First1, _UAns));
	}

		// FUNCTION TEMPLATE find_end
template<class _FwdIt1,
	class _FwdIt2> inline
	_FwdIt1 find_end(_FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2)
	{	// find last [_First2, _Last2) match
	return (find_end(_First1, _Last1, _First2, _Last2, std::equal_to<>()));
	}

		// FUNCTION TEMPLATE find_first_of WITH PRED
template<class _FwdIt1,
	class _FwdIt2,
	class _Pr> inline
	_FwdIt1 find_first_of(_FwdIt1 _First1, _FwdIt1 _Last1,
		_FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred)
	{	// look for one of [_First2, _Last2) satisfying _Pred with element
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	const auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	for (; _UFirst1 != _ULast1; ++_UFirst1)
		{
		for (auto _UMid2 = _UFirst2; _UMid2 != _ULast2; ++_UMid2)
			{
			if (_Pred(*_UFirst1, *_UMid2))
				{
				return (_Rechecked(_First1, _UFirst1));
				}
			}
		}

	return (_Rechecked(_First1, _UFirst1));
	}

		// FUNCTION TEMPLATE find_first_of
template<class _FwdIt1,
	class _FwdIt2> inline
	_FwdIt1 find_first_of(_FwdIt1 _First1, _FwdIt1 _Last1,
		_FwdIt2 _First2, _FwdIt2 _Last2)
	{	// look for one of [_First2, _Last2) that matches element
	return (find_first_of(_First1, _Last1, _First2, _Last2, std::equal_to<>()));
	}

		// FUNCTION TEMPLATE swap_ranges
template<class _FwdIt1,
	class _FwdIt2> inline
	_FwdIt2 _Swap_ranges_unchecked(_FwdIt1 _First1, _FwdIt1 _Last1,
		_FwdIt2 _Dest)
	{	// swap [_First1, _Last1) with [_Dest, ...)
	for (; _First1 != _Last1; ++_First1, (void)++_Dest)
		_STD iter_swap(_First1, _Dest);
	return (_Dest);
	}

template<class _FwdIt1,
	class _FwdIt2> inline
	_FwdIt2 swap_ranges(_FwdIt1 _First1, _FwdIt1 _Last1,
		_FwdIt2 _Dest)
	{	// swap [_First1, _Last1) with [_Dest, ...)
	_DEPRECATE_UNCHECKED(swap_ranges, _Dest);
	_DEBUG_RANGE(_First1, _Last1);
	const auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	const auto _UDest = std::_Unchecked_n(_Dest, std::_Idl_distance<_FwdIt1>(_UFirst1, _ULast1));
	return (_Rechecked(_Dest,
		_Swap_ranges_unchecked(_UFirst1, _ULast1, _UDest)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _FwdIt1,
	class _DestTy,
	size_t _DestSize> inline
	_DestTy *swap_ranges(_FwdIt1 _First1, _FwdIt1 _Last1, _DestTy (&_Dest)[_DestSize])
	{	// swap [_First1, _Last1) with [_Dest, ...), array dest
	return (_Unchecked(
		_STD swap_ranges(_First1, _Last1,
		_Array_iterator<_DestTy, _DestSize>(_Dest))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

#if _HAS_CXX17
template<class _ExPo,
	class _FwdIt1,
	class _FwdIt2,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	_FwdIt2 swap_ranges(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _Dest) noexcept // Enforces termination
	{	// swap [_First1, _Last1) with [_Dest, ...)
		// not parallelized as benchmarks show it isn't worth it
	return (swap_ranges(_First1, _Last1, _Dest));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _ExPo,
	class _FwdIt1,
	class _DestTy,
	size_t _DestSize,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	_DestTy *swap_ranges(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1,
		_DestTy (&_Dest)[_DestSize]) noexcept // Enforces termination
	{	// swap [_First1, _Last1) with [_Dest, ...), array dest
		// not parallelized as benchmarks show it isn't worth it
	return (_STD swap_ranges(_First1, _Last1, _Dest));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */
#endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE transform WITH UNARY OP
template<class _InIt,
	class _OutIt,
	class _Fn> inline
	_OutIt _Transform_no_deprecate(_InIt _First, _InIt _Last,
		_OutIt _Dest, _Fn _Func)
	{	// transform [_First, _Last) with _Func, no deprecation warnings
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest = std::_Unchecked_n(_Dest, std::_Idl_distance<_InIt>(_UFirst, _ULast));
	for (; _UFirst != _ULast; ++_UFirst, (void)++_UDest)
		{
		*_UDest = _Func(*_UFirst);
		}

	return (_Rechecked(_Dest, _UDest));
	}

template<class _InIt,
	class _OutIt,
	class _Fn> inline
	_OutIt transform(_InIt _First, _InIt _Last,
		_OutIt _Dest, _Fn _Func)
	{	// transform [_First, _Last) with _Func
	_DEPRECATE_UNCHECKED(transform, _Dest);
	return (_Transform_no_deprecate(_First, _Last, _Dest, _Pass_fn(_Func)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt,
	class _DestTy,
	size_t _DestSize,
	class _Fn> inline
	_DestTy *transform(_InIt _First, _InIt _Last, _DestTy (&_Dest)[_DestSize], _Fn _Func)
	{	// transform [_First, _Last) with _Func, array dest
	return (_Unchecked(
		_Transform_no_deprecate(_First, _Last,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Func))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE transform WITH BINARY OP
template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Fn> inline
	_OutIt _Transform_no_deprecate(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _OutIt _Dest, _Fn _Func)
	{	// transform [_First1, _Last1) and [_First2, ...) with _Func, no deprecation warnings
	_DEBUG_RANGE(_First1, _Last1);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	const auto _Count = std::_Idl_distance<_InIt1>(_UFirst1, _ULast1);
	auto _UFirst2 = std::_Unchecked_n(_First2, _Count);
	auto _UDest = std::_Unchecked_n(_Dest, _Count);
	for (; _UFirst1 != _ULast1; ++_UFirst1, (void)++_UFirst2, ++_UDest)
		{
		*_UDest = _Func(*_UFirst1, *_UFirst2);
		}

	return (_Rechecked(_Dest, _UDest));
	}

template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Fn> inline
	_OutIt transform(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _OutIt _Dest, _Fn _Func)
	{	// transform [_First1, _Last1) and [_First2, ...) with _Func
	return (_Transform_no_deprecate(_First1, _Last1, _First2, _Dest, _Pass_fn(_Func)));
	}


		// FUNCTION TEMPLATE replace
template<class _FwdIt,
	class _Ty> inline
	void replace(_FwdIt _First, _FwdIt _Last, const _Ty& _Oldval, const _Ty& _Newval)
	{	// replace each matching _Oldval with _Newval
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (*_UFirst == _Oldval)
			{
			*_UFirst = _Newval;
			}
		}
	}

#if _HAS_CXX17
template<class _ExPo,
	class _FwdIt,
	class _Ty,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	void replace(_ExPo&& _Exec,
		_FwdIt _First, _FwdIt _Last, const _Ty& _Oldval, const _Ty& _Newval) noexcept;
#endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE replace_if
template<class _FwdIt,
	class _Pr,
	class _Ty> inline
	void replace_if(_FwdIt _First, _FwdIt _Last, _Pr _Pred, const _Ty& _Val)
	{	// replace each satisfying _Pred with _Val
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (_Pred(*_UFirst))
			{
			*_UFirst = _Val;
			}
		}
	}

#if _HAS_CXX17
template<class _ExPo,
	class _FwdIt,
	class _Pr,
	class _Ty,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	void replace_if(_ExPo&& _Exec,
		_FwdIt _First, _FwdIt _Last, _Pr _Pred, const _Ty& _Val) noexcept;
#endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE replace_copy
template<class _InIt,
	class _OutIt,
	class _Ty> inline
	_OutIt replace_copy(_InIt _First, _InIt _Last,
		_OutIt _Dest, const _Ty& _Oldval, const _Ty& _Newval)
	{	// copy replacing each matching _Oldval with _Newval
	_DEPRECATE_UNCHECKED(replace_copy, _Dest);
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest = std::_Unchecked_n(_Dest, std::_Idl_distance<_InIt>(_UFirst, _ULast));
	for (; _UFirst != _ULast; ++_UFirst, (void)++_UDest)
		{
		if (*_UFirst == _Oldval)
			{
			*_UDest = _Newval;
			}
		else
			{
			*_UDest = *_UFirst;
			}
		}
	return (_Rechecked(_Dest, _UDest));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt,
	class _DestTy,
	size_t _DestSize,
	class _Ty> inline
	_DestTy *replace_copy(_InIt _First, _InIt _Last,
		_DestTy (&_Dest)[_DestSize], const _Ty& _Oldval, const _Ty& _Newval)
	{	// copy replacing each matching _Oldval with _Newval, array dest
	return (_Unchecked(
		_STD replace_copy(_First, _Last,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Oldval, _Newval)));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE replace_copy_if
template<class _InIt,
	class _OutIt,
	class _Pr,
	class _Ty> inline
	_OutIt _Replace_copy_if_no_deprecate(_InIt _First, _InIt _Last,
		_OutIt _Dest, _Pr _Pred, const _Ty& _Val)
	{	// copy replacing each satisfying _Pred with _Val, no deprecation warnings
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest = std::_Unchecked_n(_Dest, std::_Idl_distance<_InIt>(_UFirst, _ULast));
	for (; _UFirst != _ULast; ++_UFirst, (void)++_UDest)
		{
		if (_Pred(*_UFirst))
			{
			*_UDest = _Val;
			}
		else
			{
			*_UDest = *_UFirst;
			}
		}

	return (_Rechecked(_Dest, _UDest));
	}

template<class _InIt,
	class _OutIt,
	class _Pr,
	class _Ty> inline
	_OutIt replace_copy_if(_InIt _First, _InIt _Last,
		_OutIt _Dest, _Pr _Pred, const _Ty& _Val)
	{	// copy replacing each satisfying _Pred with _Val
	_DEPRECATE_UNCHECKED(replace_copy_if, _Dest);
	return (_Replace_copy_if_no_deprecate(_First, _Last, _Dest, _Pass_fn(_Pred), _Val));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt,
	class _DestTy,
	size_t _DestSize,
	class _Pr,
	class _Ty> inline
	_DestTy *replace_copy_if(_InIt _First, _InIt _Last,
		_DestTy (&_Dest)[_DestSize], _Pr _Pred, const _Ty& _Val)
	{	// copy replacing each satisfying _Pred with _Val, array dest
	return (_Unchecked(
		_Replace_copy_if_no_deprecate(_First, _Last,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Pred), _Val)));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE generate
template<class _FwdIt,
	class _Fn> inline
	void generate(_FwdIt _First, _FwdIt _Last, _Fn _Func)
	{	// replace [_First, _Last) with _Func()
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		*_UFirst = _Func();
		}
	}

		// FUNCTION TEMPLATE generate_n
template<class _OutIt,
	class _Diff,
	class _Fn> inline
	_OutIt generate_n(_OutIt _Dest, const _Diff _Count_raw, _Fn _Func)
	{	// replace [_Dest, _Dest + _Count) with _Func()
	std::_Algorithm_int_t<_Diff> _Count = _Count_raw;
	auto _UDest = std::_Unchecked_n(_Dest, _Count);
	for (; 0 < _Count; --_Count, (void)++_UDest)
		{
		*_UDest = _Func();
		}

	return (_Rechecked(_Dest, _UDest));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _DestTy,
	size_t _DestSize,
	class _Diff,
	class _Fn> inline
	_DestTy *generate_n(_DestTy (&_Dest)[_DestSize], const _Diff _Count_raw, _Fn _Func)
	{	// replace [_Dest, _Dest + _Count) with _Func(), array dest
	std::_Algorithm_int_t<_Diff> _Count = _Count_raw;
	_DEBUG_ARRAY_SIZE(_Dest, _Count);
	_DestTy * _UDest = _Dest;
	for (; 0 < _Count; --_Count, (void)++_UDest)
		{
		*_UDest = _Func();
		}

	return (_UDest);
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE remove_copy
template<class _InIt,
	class _OutIt,
	class _Ty> inline
	_OutIt remove_copy(_InIt _First, _InIt _Last, _OutIt _Dest, const _Ty& _Val)
	{	// copy omitting each matching _Val
	_DEPRECATE_UNCHECKED(remove_copy, _Dest);
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest = std::_Unchecked_idl0(_Dest);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (!(*_UFirst == _Val))
			{
			*_UDest = *_UFirst;
			++_UDest;
			}
		}

	return (_Rechecked(_Dest, _UDest));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt,
	class _DestTy,
	size_t _DestSize,
	class _Ty> inline
	_DestTy *remove_copy(_InIt _First, _InIt _Last,
		_DestTy (&_Dest)[_DestSize], const _Ty& _Val)
	{	// copy omitting each matching _Val, array dest
	return (_Unchecked(
		_STD remove_copy(_First, _Last,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Val)));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE remove_copy_if
template<class _InIt,
	class _OutIt,
	class _Pr> inline
	_OutIt _Remove_copy_if_no_deprecate(_InIt _First, _InIt _Last,
		_OutIt _Dest, _Pr _Pred)
	{	// copy omitting each element satisfying _Pred, no deprecation warnings
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest = std::_Unchecked_idl0(_Dest);
	for (; _UFirst != _ULast; ++_UFirst)
		{
		if (!_Pred(*_UFirst))
			{
			*_UDest = *_UFirst;
			++_UDest;
			}
		}

	return (_Rechecked(_Dest, _UDest));
	}

template<class _InIt,
	class _OutIt,
	class _Pr> inline
	_OutIt remove_copy_if(_InIt _First, _InIt _Last,
		_OutIt _Dest, _Pr _Pred)
	{	// copy omitting each element satisfying _Pred
	_DEPRECATE_UNCHECKED(remove_copy_if, _Dest);
	return (_Remove_copy_if_no_deprecate(_First, _Last, _Dest, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt,
	class _DestTy,
	size_t _DestSize,
	class _Pr> inline
	_DestTy *remove_copy_if(_InIt _First, _InIt _Last,
		_DestTy (&_Dest)[_DestSize], _Pr _Pred)
	{	// copy omitting each element satisfying _Pred, array dest
	return (_Unchecked(
		_Remove_copy_if_no_deprecate(_First, _Last,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE remove
template<class _FwdIt,
	class _Ty> inline
	_FwdIt _Remove_unchecked(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
	{	// remove each matching _Val
	_First = _Find_unchecked(_First, _Last, _Val);
	_FwdIt _Next = _First;
	if (_First != _Last)
		{
		while (++_First != _Last)
			{
			if (!(*_First == _Val))
				{
				*_Next = _STD move(*_First);
				++_Next;
				}
			}
		}

	return (_Next);
	}

template<class _FwdIt,
	class _Ty> inline
	_FwdIt remove(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
	{	// remove each matching _Val
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Remove_unchecked(_Unchecked(_First), _Unchecked(_Last), _Val)));
	}

		// FUNCTION TEMPLATE remove_if
template<class _FwdIt,
	class _Pr> inline
	_FwdIt _Remove_if_unchecked(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// remove each satisfying _Pred
	_First = _Find_if_unchecked(_First, _Last, _Pred);
	_FwdIt _Next = _First;
	if (_First != _Last)
		{
		while (++_First != _Last)
			{
			if (!_Pred(*_First))
				{
				*_Next = _STD move(*_First);
				++_Next;
				}
			}
		}

	return (_Next);
	}

template<class _FwdIt,
	class _Pr> inline
	_FwdIt remove_if(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// remove each satisfying _Pred
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Remove_if_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred))));
	}

		// FUNCTION TEMPLATE unique WITH PRED
template<class _FwdIt,
	class _Pr> inline
	_FwdIt unique(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// remove each satisfying _Pred with previous
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	if (_UFirst != _ULast)
		{
		for (auto _UFirstb = _UFirst; ++_UFirst != _ULast; _UFirstb = _UFirst)
			{
			if (_Pred(*_UFirstb, *_UFirst))
				{	// copy down
				while (++_UFirst != _ULast)
					{
					if (!_Pred(*_UFirstb, *_UFirst))
						{
						*++_UFirstb = _STD move(*_UFirst);
						}
					}

				return (_Rechecked(_Last, ++_UFirstb));
				}
			}
		}

	return (_Rechecked(_Last, _ULast));
	}

		// FUNCTION TEMPLATE unique
template<class _FwdIt> inline
	_FwdIt unique(_FwdIt _First, _FwdIt _Last)
	{	// remove each matching previous
	return (unique(_First, _Last, std::equal_to<>()));
	}

		// FUNCTION TEMPLATE unique_copy WITH PRED
template<class _FwdIt,
	class _OutIt,
	class _Pr> inline
	_OutIt _Unique_copy_unchecked(_FwdIt _First, _FwdIt _Last,
		_OutIt _Dest, _Pr _Pred, std::true_type, std::_Any_tag)
	{	// copy compressing pairs satisfying _Pred, forward source iterator
		// (can reread the source for comparison)
	if (_First != _Last)
		{
		_FwdIt _Firstb = _First;

		*_Dest = *_Firstb;
		++_Dest;

		while (++_First != _Last)
			{
			if (!_Pred(*_Firstb, *_First))
				{	// copy unmatched
				_Firstb = _First;
				*_Dest = *_Firstb;
				++_Dest;
				}
			}
		}

	return (_Dest);
	}

template<class _InIt,
	class _FwdIt,
	class _Pr> inline
	_FwdIt _Unique_copy_unchecked(_InIt _First, _InIt _Last,
		_FwdIt _Dest, _Pr _Pred, std::false_type, std::true_type)
	{	// copy compressing pairs satisfying _Pred, forward dest iterator with matching T
		// (assignment copies T; can reread dest for comparison)
	if (_First != _Last)
		{
		*_Dest = *_First;

		while (++_First != _Last)
			{
			if (!_Pred(*_Dest, *_First))
				{
				*++_Dest = *_First;
				}
			}

		++_Dest;
		}

	return (_Dest);
	}

template<class _InIt,
	class _OutIt,
	class _Pr> inline
	_OutIt _Unique_copy_unchecked(_InIt _First, _InIt _Last,
		_OutIt _Dest, _Pr _Pred, std::false_type, std::false_type)
	{	// copy compressing pairs satisfying _Pred, otherwise
		// (can't reread source or dest, construct a temporary)
	if (_First != _Last)
		{
		std::_Iter_value_t<_InIt> _Val = *_First;

		*_Dest = _Val;
		++_Dest;

		while (++_First != _Last)
			{
			if (!_Pred(_Val, *_First))
				{	// copy unmatched
				_Val = *_First;
				*_Dest = _Val;
				++_Dest;
				}
			}
		}

	return (_Dest);
	}

template<class _InIt,
	class _OutIt,
	class _Pr> inline
	_OutIt _Unique_copy_no_deprecate(_InIt _First, _InIt _Last,
		_OutIt _Dest, _Pr _Pred)
	{	// copy compressing pairs that match, no deprecation warnings
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_Dest,
		_Unique_copy_unchecked(_Unchecked(_First), _Unchecked(_Last),
			std::_Unchecked_idl0(_Dest), _Pred,
			std::is_base_of<std::forward_iterator_tag, std::_Iter_cat_t<_InIt>>(), // to avoid ambiguity
			std::_Conjunction_t<std::is_base_of<std::forward_iterator_tag, std::_Iter_cat_t<_OutIt>>,
				std::is_same<std::_Iter_value_t<_InIt>, std::_Iter_value_t<_OutIt>>>()
			)));
	}

template<class _InIt,
	class _OutIt,
	class _Pr> inline
	_OutIt unique_copy(_InIt _First, _InIt _Last,
		_OutIt _Dest, _Pr _Pred)
	{	// copy compressing pairs that match
	_DEPRECATE_UNCHECKED(unique_copy, _Dest);
	return (_Unique_copy_no_deprecate(_First, _Last, _Dest, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt,
	class _DestTy,
	size_t _DestSize,
	class _Pr> inline
	_DestTy *unique_copy(_InIt _First, _InIt _Last, _DestTy (&_Dest)[_DestSize], _Pr _Pred)
	{	// copy compressing pairs that match, array dest
	return (_Unchecked(
		_Unique_copy_no_deprecate(_First, _Last,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE unique_copy
template<class _InIt,
	class _OutIt> inline
	_OutIt unique_copy(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	// copy compressing pairs that match
	return (unique_copy(_First, _Last, _Dest, std::equal_to<>()));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt,
	class _DestTy,
	size_t _DestSize> inline
	_DestTy *unique_copy(_InIt _First, _InIt _Last, _DestTy (&_Dest)[_DestSize])
	{	// copy compressing pairs that match, array dest
	return (_STD unique_copy(_First, _Last, _Dest, std::equal_to<>()));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE reverse_copy
template<class _BidIt,
	class _OutIt> inline
	_OutIt reverse_copy(_BidIt _First, _BidIt _Last,
		_OutIt _Dest)
	{	// copy reversing elements in [_First, _Last)
	_DEPRECATE_UNCHECKED(reverse_copy, _Dest);
	_DEBUG_RANGE(_First, _Last);
	const auto _UFirst = _Unchecked(_First);
	auto _ULast = _Unchecked(_Last);
	auto _UDest = std::_Unchecked_n(_Dest, std::_Idl_distance<_BidIt>(_UFirst, _ULast));
	for (; _UFirst != _ULast; ++_UDest)
		{
		*_UDest = *--_ULast;
		}

	return (_Rechecked(_Dest, _UDest));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _BidIt,
	class _DestTy,
	size_t _DestSize> inline
	_DestTy *reverse_copy(_BidIt _First, _BidIt _Last, _DestTy (&_Dest)[_DestSize])
	{	// copy reversing elements in [_First, _Last), array dest
	return (_Unchecked(
		_STD reverse_copy(_First, _Last,
			_Array_iterator<_DestTy, _DestSize>(_Dest))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

#if _HAS_CXX17
template<class _ExPo,
	class _BidIt,
	class _FwdIt,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	_FwdIt reverse_copy(_ExPo&&, _BidIt _First, _BidIt _Last, _FwdIt _Dest) noexcept // Enforces termination
	{	// copy reversing elements in [_First, _Last)
		// not parallelized as benchmarks show it isn't worth it
	return (reverse_copy(_First, _Last, _Dest));
	}

 #endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE rotate_copy
template<class _FwdIt,
	class _OutIt> inline
	_OutIt rotate_copy(_FwdIt _First, _FwdIt _Mid, _FwdIt _Last, _OutIt _Dest)
	{	// copy rotating [_First, _Last)
	_DEBUG_RANGE(_First, _Mid);
	_DEBUG_RANGE(_Mid, _Last);
	const auto _UFirst = _Unchecked(_First);
	const auto _UMid = _Unchecked(_Mid);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest = std::_Unchecked_n(_Dest, std::_Idl_distance<_FwdIt>(_UFirst, _ULast));
	_UDest = _Copy_unchecked(_UMid, _ULast, _UDest);
	return (_Rechecked(_Dest, _Copy_unchecked(_UFirst, _UMid, _UDest)));
	}

#if _HAS_CXX17
template<class _ExPo,
	class _FwdIt1,
	class _FwdIt2,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	_FwdIt2 rotate_copy(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Mid, _FwdIt1 _Last,
		_FwdIt2 _Dest) noexcept // Enforces termination
	{	// copy rotating [_First, _Last)
		// not parallelized as benchmarks show it isn't worth it
	return (rotate_copy(_First, _Mid, _Last, _Dest));
	}

		// FUNCTION TEMPLATE sample
template<class _PopIt,
	class _SampleIt,
	class _Diff,
	class _RngFn> inline
	_SampleIt _Sample_reservoir_unchecked(_PopIt _First, const _PopIt _Last, const _SampleIt _Dest,
		const _Diff _Count, _RngFn& _RngFunc)
	{	// source is input: use reservoir sampling (unstable)
	using _UPopDiff = std::make_unsigned_t<std::_Iter_diff_t<_PopIt>>;
	const auto _UCount = static_cast<std::common_type_t<std::make_unsigned_t<_Diff>, _UPopDiff>>(_Count);
	_UPopDiff _PopSize = 0;
	for (; _PopSize < _UCount; ++_PopSize, (void)++_First)
		{
		if (_First == _Last)
			return (_Dest + _PopSize);
		_Dest[_PopSize] = *_First;
		}
	for (; _First != _Last; ++_First)
		{
		const auto _Idx = _RngFunc(++_PopSize);
		if (_Idx < _UCount)
			_Dest[_Idx] = *_First;
		}
	return (_Dest + _UCount);
	}

template<class _PopIt,
	class _SampleIt,
	class _Diff,
	class _RngFn> inline
	_SampleIt _Sample_selection_unchecked(_PopIt _First, const _PopIt _Last,
		std::make_unsigned_t<std::_Iter_diff_t<_PopIt>> _PopSize, _SampleIt _Dest,
		_Diff _Count, _RngFn& _RngFunc)
	{	// source is forward *and* we know the source range size: use selection sampling (stable)
	// Requires: _Diff is an unsigned integral type
	// Pre: _PopIt is forward && _Count <= _PopSize
	for (; _Count > 0 && _First != _Last; ++_First, (void)--_PopSize)
		{
		if (_RngFunc(_PopSize) < _Count)
			{
			--_Count;
			*_Dest = *_First;
			++_Dest;
			}
		}
	return (_Dest);
	}

template<class _PopIt,
	class _SampleIt,
	class _Diff,
	class _RngFn> inline
	_SampleIt _Sample1(_PopIt _First, _PopIt _Last, _SampleIt _Dest,
		_Diff _Count, _RngFn& _RngFunc, std::input_iterator_tag)
	{	// source is input: use reservoir sampling (unstable)
	// Pre: _Count > 0
	return (_Rechecked(_Dest,
		_Sample_reservoir_unchecked(_First, _Last, std::_Unchecked_idl0(_Dest), _Count,
		_RngFunc)));
	}

template<class _PopIt,
	class _SampleIt,
	class _Diff,
	class _RngFn> inline
	_SampleIt _Sample1(_PopIt _First, _PopIt _Last, _SampleIt _Dest,
		_Diff _Count, _RngFn& _RngFunc, std::forward_iterator_tag)
	{	// source is forward: use selection sampling (stable)
	// Pre: _Count > 0
	using _UPopDiff = std::make_unsigned_t<std::_Iter_diff_t<_PopIt>>;
	const auto _PopSize = static_cast<_UPopDiff>(_STD distance(_First, _Last));
	auto _UCount = static_cast<std::common_type_t<std::make_unsigned_t<_Diff>, _UPopDiff>>(_Count);
	if (_UCount > _PopSize)
		_UCount = _PopSize;
	return (_Rechecked(_Dest,
		_Sample_selection_unchecked(_First, _Last, _PopSize, std::_Unchecked_n(_Dest, _UCount),
		_UCount, _RngFunc)));
	}

template<class _PopIt,
	class _SampleIt,
	class _Diff,
	class _Urng> inline
	_SampleIt sample(_PopIt _First, _PopIt _Last, _SampleIt _Dest,
		_Diff _Count, _Urng&& _Func)
	{	// randomly select _Count elements from [_First, _Last) into _Dest
	static_assert(is_base_of_v<std::forward_iterator_tag, std::_Iter_cat_t<_PopIt>>
		|| is_base_of_v<std::random_access_iterator_tag, std::_Iter_cat_t<_SampleIt>>,
		"If the source range is not forward, the destination range must be random-access.");
	static_assert(is_integral_v<_Diff>,
		"The sample size must have an integer type.");
	_DEBUG_RANGE(_First, _Last);
	if (_Count <= 0)
		return (_Dest);
	using _UPopDiff = make_unsigned_t<std::_Iter_diff_t<_PopIt>>;
	_Rng_from_urng<_UPopDiff, remove_reference_t<_Urng>> _RngFunc(_Func);
	return (_Sample1(_Unchecked(_First), _Unchecked(_Last), _Dest, _Count,
		_RngFunc, std::_Iter_cat_t<_PopIt>()));
	}

#if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _PopIt,
	class _DestTy,
	size_t _DestSize,
	class _Diff,
	class _Urng> inline
	_DestTy *sample(_PopIt _First, _PopIt _Last, _DestTy (&_Dest)[_DestSize],
		_Diff _Count, _Urng&& _Func)
	{	// randomly select _Count elements from [_First, _Last) into _Dest
	return (_Unchecked(
		_STD sample(_First, _Last, _Array_iterator<_DestTy, _DestSize>(_Dest),
			_Count, _Func)));
	}
#endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */
#endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE shuffle WITH URNG
template<class _RanIt,
	class _RngFn> inline
	void _Random_shuffle1(_RanIt _First, _RanIt _Last, _RngFn& _RngFunc)
	{	// shuffle [_First, _Last) using random function _RngFunc
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	if (_UFirst == _ULast)
		{
		return;
		}

	using _Diff = std::_Iter_diff_t<std::_Unchecked_t<_RanIt>>;
	auto _UTarget = _UFirst;
	_Diff _Target_index = 1;
	for (; ++_UTarget != _ULast; ++_Target_index)
		{	// randomly place an element from [_First, _Target] at _Target
		_Diff _Off = _RngFunc(_Target_index + static_cast<_Diff>(1));
		_IDL_VERIFY(0 <= _Off && _Off <= _Target_index, "random value out of range");
		if (_Off != _Target_index)	// avoid self-move-assignment
			{
			_STD iter_swap(_UTarget, _UFirst + _Off);
			}
		}
	}

template<class _RanIt,
	class _Urng> inline
	void shuffle(_RanIt _First, _RanIt _Last, _Urng&& _Func)
	{	// shuffle [_First, _Last) using URNG _Func
	typedef std::remove_reference_t<_Urng> _Urng0;
	_Rng_from_urng<std::_Iter_diff_t<_RanIt>, _Urng0> _RngFunc(_Func);
	_Random_shuffle1(_First, _Last, _RngFunc);
	}

 
		// FUNCTION TEMPLATE partition
template<class _FwdIt,
	class _Pr> inline
	_FwdIt _Partition_unchecked(_FwdIt _First, _FwdIt _Last, _Pr _Pred,
		std::forward_iterator_tag)
	{	// move elements satisfying _Pred to front, forward iterators
	for (;;)
		{	// skip in-place elements at beginning
		if (_First == _Last)
			{
			return (_First);
			}

		if (!_Pred(*_First))
			{
			break;
			}

		++_First;
		}

	for (_FwdIt _Next = _First; ++_Next != _Last; )
		{
		if (_Pred(*_Next))
			{
			_STD iter_swap(_First, _Next);	// out of place, swap and loop
			++_First;
			}
		}

	return (_First);
	}

template<class _BidIt,
	class _Pr> inline
	_BidIt _Partition_unchecked(_BidIt _First, _BidIt _Last, _Pr _Pred,
		std::bidirectional_iterator_tag)
	{	// move elements satisfying _Pred to front, bidirectional iterators
	for (;;)
		{	// find any out-of-order std::pair
		for (;;)
			{	// skip in-place elements at beginning
			if (_First == _Last)
				{
				return (_First);
				}

			if (!_Pred(*_First))
				{
				break;
				}

			++_First;
			}

		do
			{	// skip in-place elements at end
			--_Last;
			if (_First == _Last)
				{
				return (_First);
				}
			}
		while (!_Pred(*_Last));

		_STD iter_swap(_First, _Last);	// out of place, swap and loop
		++_First;
		}
	}

template<class _FwdIt,
	class _Pr> inline
	_FwdIt partition(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// move elements satisfying _Pred to beginning of sequence
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Partition_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred),
			std::_Iter_cat_t<_FwdIt>())));
	}

		// FUNCTION TEMPLATE stable_partition
template<class _BidIt,
	class _Diff,
	class _Ty> inline
	_BidIt _Buffered_rotate_unchecked(const _BidIt _First, const _BidIt _Mid, const _BidIt _Last,
		const _Diff _Count1, const _Diff _Count2, _Temporary_buffer<_Ty>& _Temp_buf)
	{	// rotate [_First, _Last) using temp buffer
		// precondition: _Count1 == distance(_First, _Mid)
		// precondition: _Count2 == distance(_Mid, _Last)
	if (_Count1 == 0)
		{
		return (_Last);
		}

	if (_Count2 == 0)
		{
		return (_First);
		}

	if (_Count1 <= _Count2 && _Count1 <= _Temp_buf._Capacity)
		{	// buffer left range, then copy parts
		_Temporary_range<_Ty> _Temp(_Temp_buf, _First, _Mid, _Count1);
		const _BidIt _New_mid = _Move_unchecked(_Mid, _Last, _First);
		_Move_unchecked(_Temp._Begin(), _Temp._End(), _New_mid);
		return (_New_mid);
		}

	if (_Count2 <= _Temp_buf._Capacity)
		{	// buffer right range, then copy parts
		_Temporary_range<_Ty> _Temp(_Temp_buf, _Mid, _Last, _Count2);
		_Move_backward_unchecked(_First, _Mid, _Last);
		return (_Move_unchecked(_Temp._Begin(), _Temp._End(), _First));
		}

	// buffer too small, rotate in place
	return (_Rotate_unchecked(_First, _Mid, _Last));
	}

template<class _BidIt,
	class _Pr,
	class _Diff,
	class _Ty> inline
	std::pair<_BidIt, _Diff> _Stable_partition_unchecked1(_BidIt _First, _BidIt _Last, _Pr _Pred,
		const _Diff _Count, _Temporary_buffer<_Ty>& _Temp_buf)
	{	// implement stable_partition of [_First, _Last] (note: closed range)
		// precondition: !_Pred(*_First)
		// precondition: _Pred(*_Last)
		// precondition: distance(_First, _Last) + 1 == _Count
		// note: _Count >= 2 and _First != _Last
		// returns: a std::pair such that first is the partition point, and second is distance(_First, partition point)
	if (_Count - static_cast<_Diff>(1) <= _Temp_buf._Capacity) // - 1 since we never need to store *_Last
		{
		_Temporary_range<_Ty> _Temp{_Temp_buf};
		_BidIt _Next = _First;
		_Temp.push_back(_STD move(*_First));
		while (++_First != _Last)
			{	// test each element, copying to _Temp_buf if it's in the false range, or assigning backwards
				// if it's in the true range
			if (_Pred(*_First))
				{
				*_Next = _STD move(*_First);
				++_Next;
				}
			else
				{
				_Temp.push_back(_STD move(*_First));
				}
			}

		// move the last true element, *_Last, to the end of the true range
		*_Next = _STD move(*_Last);
		++_Next;
		_Move_unchecked(_Temp._Begin(), _Temp._End(), _Next);	// copy back the false range
		return (std::pair<_BidIt, _Diff>(_Next, _Count - static_cast<_Diff>(_Temp._Size)));
		}

	const _Diff _Mid_offset = _Count / static_cast<_Diff>(2); // note: >= 1 because _Count >= 2
	const _BidIt _Mid = _STD next(_First, _Mid_offset);

	// form [_First, _Left) true range, [_Left, _Mid) false range
	_BidIt _Left = _Mid;
	_Diff _Left_true_count = _Mid_offset;
	for (;;)
		{	// skip over the trailing false range before _Mid
		--_Left;
		if (_First == _Left)
			{	// the entire left range is false
			--_Left_true_count;	// to exclude *_First
			break;
			}

		if (_Pred(*_Left))
			{	// excluded the false range before _Mid, invariants reestablished, recurse
			const std::pair<_BidIt, _Diff> _Low =
				_Stable_partition_unchecked1(_First, _Left, _Pred, _Left_true_count, _Temp_buf);
			_Left = _Low.first;
			_Left_true_count = _Low.second;
			break;
			}

		--_Left_true_count;
		}

	// form [_Mid, _Right) true range, [_Right, next(_Last)) false range
	_BidIt _Right = _Mid;
	_Diff _Right_true_count = 0;
	for (;;)
		{	// skip over the leading true range after and including _Mid
		if (_Right == _Last)
			{	// the entire right range is true
			++_Right;	// to include _Last
			++_Right_true_count;
			break;
			}

		if (!_Pred(*_Right))
			{	// excluded the true range after and including _Mid, invariants reestablished, recurse
			const _Diff _Right_count = _Count - _Mid_offset;
			const _Diff _Remaining = _Right_count - _Right_true_count;
			const std::pair<_BidIt, _Diff> _High =
				_Stable_partition_unchecked1(_Right, _Last, _Pred, _Remaining, _Temp_buf);
			_Right = _High.first;
			_Right_true_count += _High.second;
			break;
			}

		++_Right;
		++_Right_true_count;
		}

	// swap the [_Left, _Mid) false range with the [_Mid, _Right) true range
	const _BidIt _Partition_point = _Buffered_rotate_unchecked(_Left, _Mid, _Right,
		_Mid_offset - _Left_true_count, _Right_true_count, _Temp_buf);
	return (std::pair<_BidIt, _Diff>(_Partition_point, _Left_true_count + _Right_true_count));
	}

template<class _BidIt,
	class _Pr> inline
	_BidIt _Stable_partition_unchecked(_BidIt _First, _BidIt _Last, _Pr _Pred)
	{	// partition preserving order of equivalents, using _Pred
	for (;;)
		{
		if (_First == _Last)
			{	// the input range range is true (already partitioned)
			return (_First);
			}

		if (!_Pred(*_First))
			{	// excluded the leading true range
			break;
			}

		++_First;
		}

	for (;;)
		{
		--_Last;
		if (_First == _Last)
			{	// the input range is already partitioned
			return (_First);
			}

		if (_Pred(*_Last))
			{	// excluded the trailing false range
			break;
			}
		}

	using _Diff = std::_Iter_diff_t<_BidIt>;
	const _Diff _Temp_count = _STD distance(_First, _Last); // _Total_count - 1 since we never need to store *_Last
	const _Diff _Total_count = _Temp_count + static_cast<_Diff>(1);
	_Temporary_buffer<std::_Iter_value_t<_BidIt>> _Temp_buf{_Temp_count};
	return (_Stable_partition_unchecked1(_First, _Last, _Pred, _Total_count, _Temp_buf).first);
	}

template<class _BidIt,
	class _Pr> inline
	_BidIt stable_partition(_BidIt _First, _BidIt _Last, _Pr _Pred)
	{	// partition preserving order of equivalents, using _Pred
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Stable_partition_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred))));
	}

		// FUNCTION TEMPLATE push_heap WITH PRED
template<class _RanIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Push_heap_by_index(_RanIt _First, _Diff _Hole,
		_Diff _Top, _Ty&& _Val, _Pr _Pred)
	{	// percolate _Hole to _Top or where _Val belongs, using _Pred
	for (_Diff _Idx = (_Hole - 1) >> 1;	// TRANSITION, VSO#433486
		_Top < _Hole && _DEBUG_LT_PRED(_Pred, *(_First + _Idx), _Val);
		_Idx = (_Hole - 1) >> 1)	// TRANSITION, VSO#433486
		{	// move _Hole up to parent
		*(_First + _Hole) = _STD move(*(_First + _Idx));
		_Hole = _Idx;
		}

	*(_First + _Hole) = _STD move(_Val);	// drop _Val into final hole
	}

template<class _RanIt,
	class _Pr> inline
	void push_heap(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// push *(_Last - 1) onto heap at [_First, _Last - 1), using _Pred
	_DEBUG_RANGE(_First, _Last);
	const auto _UFirst = _Unchecked(_First);
	auto _ULast = _Unchecked(_Last);
	using _URanIt = std::_Unchecked_t<_RanIt>;
	using _Diff = std::_Iter_diff_t<_URanIt>;
	_Diff _Count = _ULast - _UFirst;
	if (2 <= _Count)
		{
		std::_Iter_value_t<_URanIt> _Val = _STD move(*--_ULast);
		_Push_heap_by_index(_UFirst, --_Count, _Diff(0), _STD move(_Val), _Pass_fn(_Pred));
		}
	}

		// FUNCTION TEMPLATE push_heap
template<class _RanIt> inline
	void push_heap(_RanIt _First, _RanIt _Last)
	{	// push *(_Last - 1) onto heap at [_First, _Last - 1), using operator<
	_STD push_heap(_First, _Last, std::less<>());
	}

		// FUNCTION TEMPLATE pop_heap WITH PRED
template<class _RanIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Pop_heap_hole_by_index(_RanIt _First, _Diff _Hole, _Diff _Bottom,
		_Ty&& _Val, _Pr _Pred)
	{	// percolate _Hole to _Bottom, then push _Val, using _Pred
		// precondition: _Bottom != 0
	const _Diff _Top = _Hole;
	_Diff _Idx = _Hole;

	// Check whether _Idx can have a child before calculating that child's index, since
	// calculating the child's index can trigger integer overflows
	const _Diff _Max_sequence_non_leaf = (_Bottom - 1) >> 1;	// TRANSITION, VSO#433486
	while (_Idx < _Max_sequence_non_leaf)
		{	// move _Hole down to larger child
		_Idx = 2 * _Idx + 2;
		if (_DEBUG_LT_PRED(_Pred, *(_First + _Idx), *(_First + (_Idx - 1))))
			--_Idx;
		*(_First + _Hole) = _STD move(*(_First + _Idx));
		_Hole = _Idx;
		}

	if (_Idx == _Max_sequence_non_leaf && _Bottom % 2 == 0)
		{	// only child at bottom, move _Hole down to it
		*(_First + _Hole) = _STD move(*(_First + (_Bottom - 1)));
		_Hole = _Bottom - 1;
		}

	_Push_heap_by_index(_First, _Hole, _Top, _STD move(_Val), _Pred);
	}

template<class _RanIt,
	class _Ty,
	class _Pr> inline
	void _Pop_heap_hole_unchecked(_RanIt _First, _RanIt _Last, _RanIt _Dest,
		_Ty&& _Val, _Pr _Pred)
	{	// pop *_First to *_Dest and reheap, using _Pred
		// precondition: _First != _Last
		// precondition: _First != _Dest
	*_Dest = _STD move(*_First);
	_Pop_heap_hole_by_index(_First, std::_Iter_diff_t<_RanIt>(0), std::_Iter_diff_t<_RanIt>(_Last - _First),
		_STD move(_Val), _Pred);
	}

template<class _RanIt,
	class _Pr> inline
	void _Pop_heap_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// pop *_First to *(_Last - 1) and reheap, using _Pred
	if (2 <= _Last - _First)
		{
		--_Last;
		std::_Iter_value_t<_RanIt> _Val = _STD move(*_Last);
		_Pop_heap_hole_unchecked(_First, _Last, _Last, _STD move(_Val), _Pred);
		}
	}

template<class _RanIt,
	class _Pr> inline
	void pop_heap(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// pop *_First to *(_Last - 1) and reheap, using _Pred
	_DEBUG_RANGE(_First, _Last);
	_Pop_heap_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred));
	}

		// FUNCTION TEMPLATE pop_heap
template<class _RanIt> inline
	void pop_heap(_RanIt _First, _RanIt _Last)
	{	// pop *_First to *(_Last - 1) and reheap, using operator<
	_STD pop_heap(_First, _Last, std::less<>());
	}

		// FUNCTION TEMPLATE make_heap WITH PRED
template<class _RanIt,
	class _Pr> inline
	void _Make_heap_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// make nontrivial [_First, _Last) into a heap, using _Pred
	std::_Iter_diff_t<_RanIt> _Bottom = _Last - _First;
	for (std::_Iter_diff_t<_RanIt> _Hole = _Bottom >> 1; 0 < _Hole; )	// TRANSITION, VSO#433486
		{	// reheap top half, bottom to top
		--_Hole;
		std::_Iter_value_t<_RanIt> _Val = _STD move(*(_First + _Hole));
		_Pop_heap_hole_by_index(_First, _Hole, _Bottom, _STD move(_Val), _Pred);
		}
	}

template<class _RanIt,
	class _Pr> inline
	void make_heap(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// make [_First, _Last) into a heap, using _Pred
	_DEBUG_RANGE(_First, _Last);
	_Make_heap_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred));
	}

		// FUNCTION TEMPLATE make_heap
template<class _RanIt> inline
	void make_heap(_RanIt _First, _RanIt _Last)
	{	// make [_First, _Last) into a heap, using operator<
	_STD make_heap(_First, _Last, std::less<>());
	}

		// FUNCTION TEMPLATES is_heap AND is_heap_until WITH PRED
template<class _RanIt,
	class _Pr> inline
	_RanIt _Is_heap_until_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// find extent of range that is a heap ordered by _Pred
	const std::_Iter_diff_t<_RanIt> _Size = _Last - _First;
	for (std::_Iter_diff_t<_RanIt> _Off = 1; _Off < _Size; ++_Off)
		{
		if (_DEBUG_LT_PRED(_Pred, _First[(_Off - 1) >> 1], _First[_Off]))	// TRANSITION, VSO#433486
			{
			return (_First + _Off);
			}
		}

	return (_Last);
	}

template<class _RanIt,
	class _Pr> inline
	_RanIt is_heap_until(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// find extent of range that is a heap ordered by _Pred
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Is_heap_until_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred))));
	}

template<class _RanIt,
	class _Pr> inline
	bool is_heap(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// test if range is a heap ordered by _Pred
	_DEBUG_RANGE(_First, _Last);
	const auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	return (_Is_heap_until_unchecked(_UFirst, _ULast, _Pass_fn(_Pred)) == _ULast);
	}

		// FUNCTION TEMPLATES is_heap AND is_heap_until
template<class _RanIt> inline
	_RanIt is_heap_until(_RanIt _First, _RanIt _Last)
	{	// find extent of range that is a heap ordered by operator<
	return (_STD is_heap_until(_First, _Last, std::less<>()));
	}

template<class _RanIt> inline
	bool is_heap(_RanIt _First, _RanIt _Last)
	{	// test if range is a heap ordered by operator<
	return (_STD is_heap(_First, _Last, std::less<>()));
	}

		// FUNCTION TEMPLATE sort_heap WITH PRED
template<class _RanIt,
	class _Pr> inline
	void _Sort_heap_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// order heap by repeatedly popping, using _Pred
	for (; 2 <= _Last - _First; --_Last)
		{
		_Pop_heap_unchecked(_First, _Last, _Pred);
		}
	}

template<class _RanIt,
	class _Pr> inline
	void sort_heap(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// order heap by repeatedly popping, using _Pred
	_DEBUG_RANGE(_First, _Last);
	const auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
#if _ITERATOR_DEBUG_LEVEL == 2
	const auto _Counterexample = _Is_heap_until_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));
	if (_Counterexample != _ULast)
		{
		_DEBUG_ERROR("invalid heap in sort_heap()");
		}
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */
	_Sort_heap_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));
	}

		// FUNCTION TEMPLATE sort_heap
template<class _RanIt> inline
	void sort_heap(_RanIt _First, _RanIt _Last)
	{	// order heap by repeatedly popping, using operator<
	sort_heap(_First, _Last, std::less<>());
	}

		// FUNCTION TEMPLATE lower_bound WITH PRED
template<class _FwdIt,
	class _Ty,
	class _Pr> inline
	_FwdIt _Lower_bound_unchecked(_FwdIt _First, _FwdIt _Last,
		const _Ty& _Val, _Pr _Pred)
	{	// find first element not before _Val, using _Pred
	std::_Iter_diff_t<_FwdIt> _Count = _STD distance(_First, _Last);

	while (0 < _Count)
		{	// divide and conquer, find half that contains answer
		std::_Iter_diff_t<_FwdIt> _Count2 = _Count >> 1; // TRANSITION, VSO#433486
		const auto _Mid = _STD next(_First, _Count2);
		if (_Pred(*_Mid, _Val))
			{	// try top half
			_First = _Next_iter(_Mid);
			_Count -= _Count2 + 1;
			}
		else
			{
			_Count = _Count2;
			}
		}

	return (_First);
	}

template<class _FwdIt,
	class _Ty,
	class _Pr> inline
	_FwdIt lower_bound(_FwdIt _First, _FwdIt _Last,
		const _Ty& _Val, _Pr _Pred)
	{	// find first element not before _Val, using _Pred
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Lower_bound_unchecked(_Unchecked(_First), _Unchecked(_Last), _Val, _Pass_fn(_Pred))));
	}

		// FUNCTION TEMPLATE lower_bound
template<class _FwdIt,
	class _Ty> inline
	_FwdIt lower_bound(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
	{	// find first element not before _Val, using operator<
	return (_STD lower_bound(_First, _Last, _Val, std::less<>()));
	}

		// FUNCTION TEMPLATE upper_bound WITH PRED
template<class _FwdIt,
	class _Ty,
	class _Pr> inline
	_FwdIt _Upper_bound_unchecked(_FwdIt _First, _FwdIt _Last,
		const _Ty& _Val, _Pr _Pred)
	{	// find first element that _Val is before, using _Pred
	std::_Iter_diff_t<_FwdIt> _Count = _STD distance(_First, _Last);

	while (0 < _Count)
		{	// divide and conquer, find half that contains answer
		std::_Iter_diff_t<_FwdIt> _Count2 = _Count >> 1; // TRANSITION, VSO#433486
		const auto _Mid = _STD next(_First, _Count2);
		if (_Pred(_Val, *_Mid))
			{
			_Count = _Count2;
			}
		else
			{	// try top half
			_First = _Next_iter(_Mid);
			_Count -= _Count2 + 1;
			}
		}

	return (_First);
	}

template<class _FwdIt,
	class _Ty,
	class _Pr> inline
	_FwdIt upper_bound(_FwdIt _First, _FwdIt _Last,
		const _Ty& _Val, _Pr _Pred)
	{	// find first element that _Val is before, using _Pred
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Upper_bound_unchecked(_Unchecked(_First), _Unchecked(_Last), _Val, _Pass_fn(_Pred))));
	}

		// FUNCTION TEMPLATE upper_bound
template<class _FwdIt,
	class _Ty> inline
	_FwdIt upper_bound(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
	{	// find first element that _Val is before, using operator<
	return (_STD upper_bound(_First, _Last, _Val, std::less<>()));
	}

		// FUNCTION TEMPLATE equal_range WITH PRED
template<class _FwdIt,
	class _Ty,
	class _Pr> inline
	std::pair<_FwdIt, _FwdIt>
		equal_range(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, _Pr _Pred)
	{	// find range equivalent to _Val, using _Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);

	using _UFwdIt = std::_Unchecked_t<_FwdIt>;
	using _Diff = std::_Iter_diff_t<_UFwdIt>;
	_Diff _Count = _STD distance(_UFirst, _ULast);

	while (0 < _Count)
		{	// divide and conquer, check midpoint
		_Diff _Count2 = _Count >> 1; // TRANSITION, VSO#433486
		const auto _UMid = _STD next(_UFirst, _Count2);
		if (_DEBUG_LT_PRED(_Pred, *_UMid, _Val))
			{	// range begins above _UMid, loop
			_UFirst = _Next_iter(_UMid);
			_Count -= _Count2 + 1;
			}
		else if (_Pred(_Val, *_UMid))
			{
			_Count = _Count2;	// range in first half, loop
			}
		else
			{	// range straddles _UMid, find each end and return
			auto _UFirst2 = _Lower_bound_unchecked(_UFirst, _UMid, _Val, _Pass_fn(_Pred));
			_STD advance(_UFirst, _Count);
			auto _ULast2 = _Upper_bound_unchecked(_Next_iter(_UMid), _UFirst, _Val, _Pass_fn(_Pred));
			return {_Rechecked(_First, _UFirst2), _Rechecked(_Last, _ULast2)};
			}
		}

	return {_Rechecked(_First, _UFirst), _Rechecked(_Last, _UFirst)};	// empty range
	}

		// FUNCTION TEMPLATE equal_range
template<class _FwdIt,
	class _Ty> inline
	std::pair<_FwdIt, _FwdIt>
		equal_range(_FwdIt _First, _FwdIt _Last,
			const _Ty& _Val)
	{	// find range equivalent to _Val, using operator<
	return (_STD equal_range(_First, _Last, _Val, std::less<>()));
	}

		// FUNCTION TEMPLATE binary_search WITH PRED
template<class _FwdIt,
	class _Ty,
	class _Pr> inline
	bool binary_search(_FwdIt _First, _FwdIt _Last,
		const _Ty& _Val, _Pr _Pred)
	{	// test if _Val equivalent to some element, using _Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	_UFirst = _Lower_bound_unchecked(_UFirst, _ULast, _Val, _Pass_fn(_Pred));
	return (_UFirst != _ULast && !_Pred(_Val, *_UFirst));
	}

		// FUNCTION TEMPLATE binary_search
template<class _FwdIt,
	class _Ty> inline
	bool binary_search(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
	{	// test if _Val equivalent to some element, using operator<
	return (_STD binary_search(_First, _Last, _Val, std::less<>()));
	}

		// FUNCTION TEMPLATE merge WITH PRED
inline _Distance_unknown _Idl_dist_add(_Distance_unknown, _Distance_unknown)
	{	// combine std::_Idl_distance results (both unknown)
	return {};
	}

template<class _Diff1> inline
	_Distance_unknown _Idl_dist_add(_Diff1, _Distance_unknown)
	{	// combine std::_Idl_distance results (right unknown)
	return {};
	}

template<class _Diff2> inline
	_Distance_unknown _Idl_dist_add(_Distance_unknown, _Diff2)
	{	// combine std::_Idl_distance results (left unknown)
	return {};
	}

template<class _Diff1,
	class _Diff2> inline
	auto _Idl_dist_add(_Diff1 _Lhs, _Diff2 _Rhs)
	{	// combine std::_Idl_distance results (both known)
	return (_Lhs + _Rhs);
	}

template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt _Merge_no_deprecate(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest, _Pr _Pred)
	{	// copy merging ranges, both using _Pred, no deprecation warnings
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	_DEBUG_ORDER_UNCHECKED(_UFirst1, _ULast1, _Pred);
	_DEBUG_ORDER_UNCHECKED(_UFirst2, _ULast2, _Pred);
	const auto _Count1 = std::_Idl_distance<_InIt1>(_UFirst1, _ULast1);
	const auto _Count2 = std::_Idl_distance<_InIt2>(_UFirst2, _ULast2);
	auto _UDest = std::_Unchecked_n(_Dest, _Idl_dist_add(_Count1, _Count2));
	if (_UFirst1 != _ULast1 && _UFirst2 != _ULast2)
		{
		for (;;)
			{	// merge either first or second
			if (_DEBUG_LT_PRED(_Pred, *_UFirst2, *_UFirst1))
				{
				*_UDest = *_UFirst2;
				++_UDest;
				++_UFirst2;

				if (_UFirst2 == _ULast2)
					{
					break;
					}
				}
			else
				{
				*_UDest = *_UFirst1;
				++_UDest;
				++_UFirst1;

				if (_UFirst1 == _ULast1)
					{
					break;
					}
				}
			}
		}

	_UDest = _Copy_unchecked(_UFirst1, _ULast1, _UDest);	// copy any tail
	return (_Rechecked(_Dest, _Copy_unchecked(_UFirst2, _ULast2, _UDest)));
	}

template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt merge(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest, _Pr _Pred)
	{	// copy merging ranges, both using _Pred
	_DEPRECATE_UNCHECKED(merge, _Dest);
	return (_Merge_no_deprecate(_First1, _Last1, _First2, _Last2, _Dest, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize,
	class _Pr> inline
	_DestTy *merge(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_DestTy (&_Dest)[_DestSize], _Pr _Pred)
	{	// copy merging ranges, both using _Pred, array dest
	return (_Unchecked(
		_Merge_no_deprecate(_First1, _Last1,
			_First2, _Last2,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE merge
template<class _InIt1,
	class _InIt2,
	class _OutIt> inline
	_OutIt merge(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest)
	{	// copy merging ranges, both using operator<
	return (_STD merge(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize> inline
	_DestTy *merge(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_DestTy (&_Dest)[_DestSize])
	{	// copy merging ranges, both using operator<, array dest
	return (_STD merge(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE inplace_merge WITH PRED
		// The "usual invariants" for the inplace_merge helpers below are:
		// [_First, _Mid) and [_Mid, _Last) are sorted
		// _Pred(*_Mid, *_First)             note: this means *_Mid is the "lowest" element
		// _Pred(*prev(_Last), *prev(_Mid))  note: this means *prev(_Mid) is the "highest" element
		// _Count1 == distance(_First, _Mid)
		// _Count2 == distance(_Mid, _Last)
		// _Count1 > 1
		// _Count2 > 1
template<class _BidIt> inline
	void _Rotate_one_right(_BidIt _First, _BidIt _Mid, _BidIt _Last)
	{	// exchanges the range [_First, _Mid) with [_Mid, _Last)
		// pre: distance(_Mid, _Last) is 1
	std::_Iter_value_t<_BidIt> _Temp(_STD move(*_Mid));
	_Move_backward_unchecked(_First, _Mid, _Last);
	*_First = _STD move(_Temp);
	}

template<class _BidIt> inline
	void _Rotate_one_left(_BidIt _First, _BidIt _Mid, _BidIt _Last)
	{	// exchanges the range [_First, _Mid) with [_Mid, _Last)
		// pre: distance(_First, _Mid) is 1
	std::_Iter_value_t<_BidIt> _Temp(_STD move(*_First));
	*_Move_unchecked(_Mid, _Last, _First) = _STD move(_Temp);
	}

template<class _BidIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Inplace_merge_buffer_left(_BidIt _First, _BidIt _Mid, _BidIt _Last,
		_Diff _Count1, _Temporary_buffer<_Ty>& _Temp_buf, _Pr _Pred)
	{	// move the range [_First, _Mid) to _Temp_buf, and merge it with [_Mid, _Last) to _First, using _Pred
		// usual invariants apply
	_Temporary_range<_Ty> _Temp(_Temp_buf, _First, _Mid, _Count1);
	_Ty * _Left_first = _Temp._Begin();
	_Ty * const _Left_last = _Temp._End() - 1; // avoid a compare with the highest element
	*_First = _STD move(*_Mid); // the lowest element is now in position
	++_First;
	++_Mid;
	for (;;)
		{
		if (_Pred(*_Mid, *_Left_first))
			{	// take element from the right partition
			*_First = _STD move(*_Mid);
			++_First;
			++_Mid;
			if (_Mid == _Last)
				{
				_Move_unchecked(_Left_first, _Temp._End(), _First);	// move any tail (and the highest element)
				return;
				}
			}
		else
			{	// take element from the left partition
			*_First = _STD move(*_Left_first);
			++_First;
			++_Left_first;
			if (_Left_first == _Left_last)
				{	// move the remaining right partition and highest element, since *_Left_first is highest
				*_Move_unchecked(_Mid, _Last, _First) = _STD move(*_Left_last);
				return;
				}
			}
		}
	}

template<class _BidIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Inplace_merge_buffer_right(_BidIt _First, _BidIt _Mid, _BidIt _Last,
		_Diff _Count2, _Temporary_buffer<_Ty>& _Temp_buf, _Pr _Pred)
	{	// move the range [_Mid, _Last) to _Temp_buf, and merge it with [_First, _Mid) to _Last, using _Pred
		// usual invariants apply
	_Temporary_range<_Ty> _Temp(_Temp_buf, _Mid, _Last, _Count2);
	*--_Last = _STD move(*--_Mid); // move the highest element into position
	_Ty * const _Right_first = _Temp._Begin();
	_Ty * _Right_last = _Temp._End() - 1;
	--_Mid;
	for (;;)
		{
		if (_Pred(*_Right_last, *_Mid))
			{	// merge from the left partition
			*--_Last = _STD move(*_Mid);
			if (_First == _Mid)
				{
				*--_Last = _STD move(*_Right_last);	// to make [_Right_first, _Right_last) a half-open range
				_Move_backward_unchecked(_Right_first, _Right_last, _Last);	// move any head (and lowest element)
				return;
				}

			--_Mid;
			}
		else
			{	// merge from the right partition
			*--_Last = _STD move(*_Right_last);
			--_Right_last;
			if (_Right_first == _Right_last)
				{	// we can't compare with *_Right_first, but we know it is lowest
				*--_Last = _STD move(*_Mid);	// restore half-open range [_First, _Mid)
				_Move_backward_unchecked(_First, _Mid, _Last);
				*_First = _STD move(*_Right_first);
				return;
				}
			}
		}
	}

template<class _BidIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Buffered_inplace_merge_unchecked(_BidIt _First, _BidIt _Mid, _BidIt _Last,
		_Diff _Count1, _Diff _Count2, _Temporary_buffer<_Ty>& _Temp_buf, _Pr _Pred);

template<class _BidIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Buffered_inplace_merge_divide_and_conquer2(_BidIt _First, _BidIt _Mid, _BidIt _Last,
		_Diff _Count1, _Diff _Count2, _Temporary_buffer<_Ty>& _Temp_buf, _Pr _Pred,
		_BidIt _Firstn, _BidIt _Lastn, _Diff _Count1n, _Diff _Count2n)
	{	// common block of _Buffered_inplace_merge_divide_and_conquer, below
	_BidIt _Midn = _Buffered_rotate_unchecked(_Firstn, _Mid, _Lastn,
		_Count1 - _Count1n, _Count2n, _Temp_buf);	// rearrange middle
	_Buffered_inplace_merge_unchecked(_First, _Firstn, _Midn,
		_Count1n, _Count2n, _Temp_buf, _Pred);	// merge each new part
	_Buffered_inplace_merge_unchecked(_Midn, _Lastn, _Last,
		_Count1 - _Count1n, _Count2 - _Count2n, _Temp_buf, _Pred);
	}

template<class _BidIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Buffered_inplace_merge_divide_and_conquer(_BidIt _First, _BidIt _Mid, _BidIt _Last,
		_Diff _Count1, _Diff _Count2, _Temporary_buffer<_Ty>& _Temp_buf, _Pr _Pred)
	{	// merge sorted [_First, _Mid) with sorted [_Mid, _Last), using _Pred
		// usual invariants apply
	if (_Count1 <= _Count2)
		{
		const _Diff _Count1n = _Count1 >> 1;	// TRANSITION, VSO#433486
		const _BidIt _Firstn = _STD next(_First, _Count1n);
		const _BidIt _Lastn = _Lower_bound_unchecked(_Mid, _Last, *_Firstn, _Pred);
		_Buffered_inplace_merge_divide_and_conquer2(_First, _Mid, _Last, _Count1, _Count2,
			_Temp_buf, _Pred,
			_Firstn, _Lastn, _Count1n, _STD distance(_Mid, _Lastn));
		}
	else
		{
		const _Diff _Count2n = _Count2 >> 1;	// TRANSITION, VSO#433486
		const _BidIt _Lastn = _STD next(_Mid, _Count2n);
		const _BidIt _Firstn = _Upper_bound_unchecked(_First, _Mid, *_Lastn, _Pred);
		_Buffered_inplace_merge_divide_and_conquer2(_First, _Mid, _Last, _Count1, _Count2,
			_Temp_buf, _Pred,
			_Firstn, _Lastn, _STD distance(_First, _Firstn), _Count2n);
		}
	}

template<class _BidIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Buffered_inplace_merge_unchecked_impl(_BidIt _First, _BidIt _Mid, _BidIt _Last,
		_Diff _Count1, _Diff _Count2, _Temporary_buffer<_Ty>& _Temp_buf, _Pr _Pred)
	{	// merge sorted [_First, _Mid) with sorted [_Mid, _Last), using _Pred
		// usual invariants apply
	if (_Count1 <= _Count2 && _Count1 <= _Temp_buf._Capacity)
		{
		_Inplace_merge_buffer_left(_First, _Mid, _Last, _Count1, _Temp_buf, _Pred);
		}
	else if (_Count2 <= _Temp_buf._Capacity)
		{
		_Inplace_merge_buffer_right(_First, _Mid, _Last, _Count2, _Temp_buf, _Pred);
		}
	else
		{
		_Buffered_inplace_merge_divide_and_conquer(_First, _Mid, _Last, _Count1, _Count2, _Temp_buf, _Pred);
		}
	}

template<class _BidIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Buffered_inplace_merge_unchecked(_BidIt _First, _BidIt _Mid, _BidIt _Last,
		_Diff _Count1, _Diff _Count2, _Temporary_buffer<_Ty>& _Temp_buf, _Pr _Pred)
	{	// merge sorted [_First, _Mid) with sorted [_Mid, _Last), using _Pred
		// usual invariants *do not* apply; only sortedness applies
		// establish the usual invariants (explained in inplace_merge)
	if (_Mid == _Last)
		{
		return;
		}

	for (;;)
		{
		if (_First == _Mid)
			{
			return;
			}

		if (_Pred(*_Mid, *_First))
			{
			break;
			}

		++_First;
		--_Count1;
		}

	const auto _Highest = _Prev_iter(_Mid);
	do
		{
		--_Last;
		--_Count2;
		if (_Mid == _Last)
			{
			_Rotate_one_right(_First, _Mid, ++_Last);
			return;
			}
		}
	while (!_Pred(*_Last, *_Highest));

	++_Last;
	++_Count2;

	if (_Count1 == 1)
		{
		_Rotate_one_left(_First, _Mid, _Last);
		return;
		}

	_Buffered_inplace_merge_unchecked_impl(_First, _Mid, _Last, _Count1, _Count2, _Temp_buf, _Pred);
	}

template<class _BidIt,
	class _Pr> inline
	void inplace_merge(_BidIt _First, _BidIt _Mid, _BidIt _Last, _Pr _Pred)
	{	// merge [_First, _Mid) with [_Mid, _Last), using _Pred
	_DEBUG_RANGE(_First, _Mid);
	_DEBUG_RANGE(_Mid, _Last);
	using _UBidIt = std::_Unchecked_t<_BidIt>;
	auto _UFirst = _Unchecked(_First);
	auto _UMid = _Unchecked(_Mid);
	auto _ULast = _Unchecked(_Last);
	_DEBUG_ORDER_UNCHECKED(_UFirst, _UMid, _Pred);

	// establish the usual invariants:
	if (_UMid == _ULast)
		{
		return;
		}

	for (;;)
		{
		if (_UFirst == _UMid)
			{
			return;
			}

		if (_Pred(*_UMid, *_UFirst))
			{	// found that *_UMid goes in *_UFirst's position
			break;
			}

		++_UFirst;
		}

	const auto _Highest = _Prev_iter(_UMid);
	do
		{
		--_ULast;
		if (_UMid == _ULast)
			{	// rotate only element remaining in right partition to the beginning, without allocating
			_Rotate_one_right(_UFirst, _UMid, ++_ULast);
			return;
			}
		}
	while (!_Pred(*_ULast, *_Highest));	// found that *_Highest goes in *_ULast's position

	++_ULast;

	const std::_Iter_diff_t<_UBidIt> _Count1 = _STD distance(_UFirst, _UMid);
	if (_Count1 == 1)
		{	// rotate only element remaining in left partition to the end, without allocating
		_Rotate_one_left(_UFirst, _UMid, _ULast);
		return;
		}

	const std::_Iter_diff_t<_UBidIt> _Count2 = _STD distance(_UMid, _ULast);
	_Temporary_buffer<std::_Iter_value_t<_UBidIt>> _Temp_buf{_Min_value(_Count1, _Count2)};
	_Buffered_inplace_merge_unchecked_impl(_UFirst, _UMid, _ULast,
		_Count1, _Count2, _Temp_buf, _Pass_fn(_Pred));
	}

		// FUNCTION TEMPLATE inplace_merge
template<class _BidIt> inline
	void inplace_merge(_BidIt _First, _BidIt _Mid, _BidIt _Last)
	{	// merge [_First, _Mid) with [_Mid, _Last), using operator<
	inplace_merge(_First, _Mid, _Last, std::less<>());
	}

		// FUNCTION TEMPLATE sort
template<class _BidIt,
	class _Pr> inline
	void _Insertion_sort_unchecked(_BidIt _First, _BidIt _Last, _Pr _Pred)
	{	// insertion sort [_First, _Last), using _Pred
	if (_First != _Last)
		{
		for (_BidIt _Next = _First; ++_Next != _Last; )
			{	// order next element
			_BidIt _Next1 = _Next;
			std::_Iter_value_t<_BidIt> _Val = _STD move(*_Next);

			if (_DEBUG_LT_PRED(_Pred, _Val, *_First))
				{	// found new earliest element, move to front
				_Move_backward_unchecked(_First, _Next, ++_Next1);
				*_First = _STD move(_Val);
				}
			else
				{	// look for insertion point after first
				for (_BidIt _First1 = _Next1;
					_DEBUG_LT_PRED(_Pred, _Val, *--_First1);
					_Next1 = _First1)
					{
					*_Next1 = _STD move(*_First1);	// move hole down
					}

				*_Next1 = _STD move(_Val);	// insert element in hole
				}
			}
		}
	}

template<class _RanIt,
	class _Pr> inline
	void _Med3_unchecked(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr _Pred)
	{	// sort median of three elements to middle
	if (_DEBUG_LT_PRED(_Pred, *_Mid, *_First))
		{
		_STD iter_swap(_Mid, _First);
		}

	if (_DEBUG_LT_PRED(_Pred, *_Last, *_Mid))
		{	// swap middle and last, then test first again
		_STD iter_swap(_Last, _Mid);

		if (_DEBUG_LT_PRED(_Pred, *_Mid, *_First))
			{
			_STD iter_swap(_Mid, _First);
			}
		}
	}

template<class _RanIt,
	class _Pr> inline
	void _Guess_median_unchecked(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr _Pred)
	{	// sort median element to middle
	if (40 < _Last - _First)
		{	// median of nine
		size_t _Step = (_Last - _First + 1) / 8;
		_Med3_unchecked(_First, _First + _Step, _First + 2 * _Step, _Pred);
		_Med3_unchecked(_Mid - _Step, _Mid, _Mid + _Step, _Pred);
		_Med3_unchecked(_Last - 2 * _Step, _Last - _Step, _Last, _Pred);
		_Med3_unchecked(_First + _Step, _Mid, _Last - _Step, _Pred);
		}
	else
		{
		_Med3_unchecked(_First, _Mid, _Last, _Pred);
		}
	}

template<class _RanIt,
	class _Pr> inline
	std::pair<_RanIt, _RanIt>
		_Partition_by_median_guess_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// partition [_First, _Last), using _Pred
	_RanIt _Mid = _First + ((_Last - _First) >> 1);	// TRANSITION, VSO#433486
	_Guess_median_unchecked(_First, _Mid, _Last - 1, _Pred);
	_RanIt _Pfirst = _Mid;
	_RanIt _Plast = _Pfirst + 1;

	while (_First < _Pfirst
		&& !_DEBUG_LT_PRED(_Pred, *(_Pfirst - 1), *_Pfirst)
		&& !_Pred(*_Pfirst, *(_Pfirst - 1)))
		{
		--_Pfirst;
		}

	while (_Plast < _Last
		&& !_DEBUG_LT_PRED(_Pred, *_Plast, *_Pfirst)
		&& !_Pred(*_Pfirst, *_Plast))
		{
		++_Plast;
		}

	_RanIt _Gfirst = _Plast;
	_RanIt _Glast = _Pfirst;

	for (;;)
		{	// partition
		for (; _Gfirst < _Last; ++_Gfirst)
			{
			if (_DEBUG_LT_PRED(_Pred, *_Pfirst, *_Gfirst))
				{
				}
			else if (_Pred(*_Gfirst, *_Pfirst))
				{
				break;
				}
			else if (_Plast != _Gfirst)
				{
				_STD iter_swap(_Plast, _Gfirst);
				++_Plast;
				}
			else
				{
				++_Plast;
				}
			}

		for (; _First < _Glast; --_Glast)
			{
			if (_DEBUG_LT_PRED(_Pred, *(_Glast - 1), *_Pfirst))
				{
				}
			else if (_Pred(*_Pfirst, *(_Glast - 1)))
				{
				break;
				}
			else if (--_Pfirst != _Glast - 1)
				{
				_STD iter_swap(_Pfirst, _Glast - 1);
				}
			}

		if (_Glast == _First && _Gfirst == _Last)
			{
			return (std::pair<_RanIt, _RanIt>(_Pfirst, _Plast));
			}

		if (_Glast == _First)
			{	// no room at bottom, rotate pivot upward
			if (_Plast != _Gfirst)
				{
				_STD iter_swap(_Pfirst, _Plast);
				}

			++_Plast;
			_STD iter_swap(_Pfirst, _Gfirst);
			++_Pfirst;
			++_Gfirst;
			}
		else if (_Gfirst == _Last)
			{	// no room at top, rotate pivot downward
			if (--_Glast != --_Pfirst)
				{
				_STD iter_swap(_Glast, _Pfirst);
				}

			_STD iter_swap(_Pfirst, --_Plast);
			}
		else
			{
			_STD iter_swap(_Gfirst, --_Glast);
			++_Gfirst;
			}
		}
	}

template<class _RanIt,
	class _Diff,
	class _Pr> inline
	void _Sort_unchecked(_RanIt _First, _RanIt _Last, _Diff _Ideal, _Pr _Pred)
	{	// order [_First, _Last), using _Pred
	_Diff _Count;
	while (_ISORT_MAX < (_Count = _Last - _First) && 0 < _Ideal)
		{	// divide and conquer by quicksort
		auto _Mid = _Partition_by_median_guess_unchecked(_First, _Last, _Pred);
		// TRANSITION, VSO#433486
		_Ideal = (_Ideal >> 1) + (_Ideal >> 2);	// allow 1.5 log2(N) divisions

		if (_Mid.first - _First < _Last - _Mid.second)
			{	// loop on second half
			_Sort_unchecked(_First, _Mid.first, _Ideal, _Pred);
			_First = _Mid.second;
			}
		else
			{	// loop on first half
			_Sort_unchecked(_Mid.second, _Last, _Ideal, _Pred);
			_Last = _Mid.first;
			}
		}

	if (_ISORT_MAX < _Count)
		{	// heap sort if too many divisions
		_Make_heap_unchecked(_First, _Last, _Pred);
		_Sort_heap_unchecked(_First, _Last, _Pred);
		}
	else if (2 <= _Count)
		{
		_Insertion_sort_unchecked(_First, _Last, _Pred);	// small
		}
	}

#if _HAS_CXX17
template<class _ExPo,
	class _RanIt,
	class _Pr,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	void sort(_ExPo&& _Exec, _RanIt _First, _RanIt _Last, _Pr _Pred) noexcept;

template<class _ExPo,
	class _RanIt,
	std::_Enable_if_execution_policy_t<_ExPo> = 0> inline
	void sort(_ExPo&& _Exec, _RanIt _First, _RanIt _Last) noexcept;
#endif /* _HAS_CXX17 */

		// FUNCTION TEMPLATE stable_sort WITH PRED
template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt _Buffered_merge_unchecked(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest, _Pr _Pred)
	{	// move merging ranges, both using _Pred
	if (_First1 != _Last1 && _First2 != _Last2)
		{
		for (;;)
			{	// merge either first or second
			if (_DEBUG_LT_PRED(_Pred, *_First2, *_First1))
				{	// merge first
				*_Dest = _STD move(*_First2);
				++_Dest;
				++_First2;

				if (_First2 == _Last2)
					{
					break;
					}
				}
			else
				{	// merge second
				*_Dest = _STD move(*_First1);
				++_Dest;
				++_First1;

				if (_First1 == _Last1)
					{
					break;
					}
				}
			}
		}

	_Dest = _Move_unchecked(_First1, _Last1, _Dest);	// move any tail
	return (_Move_unchecked(_First2, _Last2, _Dest));
	}

template<class _BidIt,
	class _OutIt,
	class _Diff,
	class _Pr> inline
	void _Chunked_merge_unchecked(_BidIt _First, _BidIt _Last, _OutIt _Dest,
		_Diff _Chunk, _Diff _Count, _Pr _Pred)
	{	// copy merging chunks, using _Pred
	for (_Diff _Chunk2 = _Chunk * 2; _Chunk2 <= _Count; _Count -= _Chunk2)
		{	// copy merging pairs of adjacent chunks
		const _BidIt _Mid1 = _STD next(_First, _Chunk);
		const _BidIt _Mid2 = _STD next(_Mid1, _Chunk);
		_Dest = _Buffered_merge_unchecked(_First, _Mid1, _Mid1, _Mid2, _Dest, _Pred);
		_First = _Mid2;
		}

	if (_Count <= _Chunk)
		{
		_Move_unchecked(_First, _Last, _Dest);	// copy partial last chunk
		}
	else
		{	// copy merging whole and partial last chunk
		const _BidIt _Mid1 = _STD next(_First, _Chunk);
		_Buffered_merge_unchecked(_First, _Mid1, _Mid1, _Last, _Dest, _Pred);
		}
	}

template<class _BidIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Buffered_merge_sort_unchecked(_BidIt _First, _BidIt _Last, _Diff _Count,
		_Temporary_buffer<_Ty>& _Temp_buf, _Pr _Pred)
	{	// sort using temp buffer for merges, using _Pred
	const auto _Diffsort_max = static_cast<_Diff>(_ISORT_MAX);
	_BidIt _Mid = _First;
	for (_Diff _Nleft = _Count; _Diffsort_max <= _Nleft; _Nleft -= _Diffsort_max)
		{	// sort chunks
		const _BidIt _Midn = _STD next(_Mid, _Diffsort_max);
		_Insertion_sort_unchecked(_Mid, _Midn, _Pred);
		_Mid = _Midn;
		}
	_Insertion_sort_unchecked(_Mid, _Last, _Pred);	// sort partial last chunk

	for (_Diff _Chunk = _Diffsort_max; _Chunk < _Count; _Chunk *= 2)
		{	// merge adjacent pairs of chunks to and from temp buffer
		_Temporary_range<_Ty> _Temp{_Temp_buf};
		_Chunked_merge_unchecked(_First, _Last, back_inserter(_Temp),
			_Chunk, _Count, _Pred);
		_Chunked_merge_unchecked(_Temp._Begin(), _Temp._End(), _First,
			_Chunk *= 2, _Count, _Pred);
		}
	}

template<class _BidIt,
	class _Diff,
	class _Ty,
	class _Pr> inline
	void _Stable_sort_unchecked(_BidIt _First, _BidIt _Last, _Diff _Count,
		_Temporary_buffer<_Ty>& _Temp_buf, _Pr _Pred)
	{	// sort preserving order of equivalents, using _Pred
	if (_Count <= _ISORT_MAX)
		{
		_Insertion_sort_unchecked(_First, _Last, _Pred);	// small
		}
	else
		{	// sort halves and merge
		_Diff _Count2 = (_Count + 1) >> 1;	// TRANSITION, VSO#433486
		const _BidIt _Mid = _STD next(_First, _Count2);
		if (_Count2 <= _Temp_buf._Capacity)
			{	// temp buffer big enough, sort each half using buffer
			_Buffered_merge_sort_unchecked(_First, _Mid, _Count2, _Temp_buf, _Pred);
			_Buffered_merge_sort_unchecked(_Mid, _Last, _Count - _Count2,
				_Temp_buf, _Pred);
			}
		else
			{	// temp buffer not big enough, divide and conquer
			_Stable_sort_unchecked(_First, _Mid, _Count2, _Temp_buf, _Pred);
			_Stable_sort_unchecked(_Mid, _Last, _Count - _Count2, _Temp_buf, _Pred);
			}

		_Buffered_inplace_merge_unchecked(_First, _Mid, _Last,
			_Count2, _Count - _Count2, _Temp_buf, _Pred);	// merge halves
		}
	}

template<class _BidIt,
	class _Pr> inline
	void stable_sort(_BidIt _First, _BidIt _Last, _Pr _Pred)
	{	// sort preserving order of equivalents, using _Pred
	_DEBUG_RANGE(_First, _Last);
	using _UBidIt = std::_Unchecked_t<_BidIt>;
	const auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	std::_Iter_diff_t<_UBidIt> _Count = _STD distance(_UFirst, _ULast);
	if (_Count == 0)
		{
		return;
		}

	if (_Count <= _ISORT_MAX)
		{
		_Insertion_sort_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));	// small
		}
	else
		{
		_Temporary_buffer<std::_Iter_value_t<_UBidIt>> _Temp_buf{(_Count + 1) >> 1};	// TRANSITION, VSO#433486
		_Stable_sort_unchecked(_UFirst, _ULast, _Count, _Temp_buf, _Pass_fn(_Pred));
		}
	}

		// FUNCTION TEMPLATE stable_sort
template<class _BidIt> inline
	void stable_sort(_BidIt _First, _BidIt _Last)
	{	// sort preserving order of equivalents, using operator<
	_STD stable_sort(_First, _Last, std::less<>());
	}

		// FUNCTION TEMPLATE partial_sort WITH PRED
template<class _RanIt,
	class _Pr> inline
	void partial_sort(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr _Pred)
	{	// order [_First, _Last) up to _Mid, using _Pred
	_DEBUG_RANGE(_First, _Mid);
	_DEBUG_RANGE(_Mid, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _UMid = _Unchecked(_Mid);
	const auto _ULast = _Unchecked(_Last);

	if (_UFirst == _UMid)
		{
		return;	// nothing to do, avoid violating _Pop_heap_hole_unchecked preconditions
		}

	_Make_heap_unchecked(_UFirst, _UMid, _Pass_fn(_Pred));
	for (auto _UNext = _UMid; _UNext < _ULast; ++_UNext)
		{
		if (_DEBUG_LT_PRED(_Pred, *_UNext, *_UFirst))
			{	// replace top with new largest
			std::_Iter_value_t<std::_Unchecked_t<_RanIt>> _Val = _STD move(*_UNext);
			_Pop_heap_hole_unchecked(_UFirst, _UMid, _UNext, _STD move(_Val), _Pass_fn(_Pred));
			}
		}

	_Sort_heap_unchecked(_UFirst, _UMid, _Pass_fn(_Pred));
	}

		// FUNCTION TEMPLATE partial_sort
template<class _RanIt> inline
	void partial_sort(_RanIt _First, _RanIt _Mid, _RanIt _Last)
	{	// order [_First, _Last) up to _Mid, using operator<
	_STD partial_sort(_First, _Mid, _Last, std::less<>());
	}

		// FUNCTION TEMPLATE partial_sort_copy WITH PRED
template<class _InIt,
	class _RanIt,
	class _Pr> inline
	_RanIt partial_sort_copy(_InIt _First1, _InIt _Last1,
		_RanIt _First2, _RanIt _Last2, _Pr _Pred)
	{	// copy [_First1, _Last1) into [_First2, _Last2) using _Pred
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	using _URanIt = std::_Unchecked_t<_RanIt>;
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	auto _UMid2 = _UFirst2;
	if (_UFirst1 != _ULast1 && _UFirst2 != _ULast2)
		{
		for (; _UFirst1 != _ULast1 && _UMid2 != _ULast2; ++_UFirst1, (void)++_UMid2)
			{
			*_UMid2 = *_UFirst1;	// copy min(_ULast1 - _UFirst1, _ULast2 - _UFirst2)
			}

		_Make_heap_unchecked(_UFirst2, _UMid2, _Pass_fn(_Pred));
		for (; _UFirst1 != _ULast1; ++_UFirst1)
			{
			if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2))
				{
				// replace top with new largest:
				_Pop_heap_hole_by_index(_UFirst2, static_cast<std::_Iter_diff_t<_URanIt>>(0),
					static_cast<std::_Iter_diff_t<_URanIt>>(_UMid2 - _UFirst2),
					static_cast<std::_Iter_value_t<std::_Unchecked_t<_InIt>>>(*_UFirst1), _Pass_fn(_Pred));
				}
			}

		_Sort_heap_unchecked(_UFirst2, _UMid2, _Pass_fn(_Pred));
		}

	return (_Rechecked(_First2, _UMid2));
	}

		// FUNCTION TEMPLATE partial_sort_copy
template<class _InIt,
	class _RanIt> inline
	_RanIt partial_sort_copy(_InIt _First1, _InIt _Last1,
		_RanIt _First2, _RanIt _Last2)
	{	// copy [_First1, _Last1) into [_First2, _Last2), using operator<
	return (_STD partial_sort_copy(_First1, _Last1, _First2, _Last2, std::less<>()));
	}

		// FUNCTION TEMPLATE nth_element WITH PRED
template<class _RanIt,
	class _Pr> inline
	void nth_element(_RanIt _First, _RanIt _Nth, _RanIt _Last, _Pr _Pred)
	{	// order Nth element, using _Pred
	_DEBUG_RANGE(_First, _Nth);
	_DEBUG_RANGE(_Nth, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _UNth = _Unchecked(_Nth);
	auto _ULast = _Unchecked(_Last);
	if (_UNth == _ULast)
		{
		return;	// nothing to do
		}

	while (_ISORT_MAX < _ULast - _UFirst)
		{	// divide and conquer, ordering partition containing Nth
		auto _UMid = _Partition_by_median_guess_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));

		if (_UMid.second <= _UNth)
			{
			_UFirst = _UMid.second;
			}
		else if (_UMid.first <= _UNth)
			{
			return;	// Nth inside fat pivot, done
			}
		else
			{
			_ULast = _UMid.first;
			}
		}

	_Insertion_sort_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));	// sort any remainder
	}

		// FUNCTION TEMPLATE nth_element
template<class _RanIt> inline
	void nth_element(_RanIt _First, _RanIt _Nth, _RanIt _Last)
	{	// order Nth element, using operator<
	_STD nth_element(_First, _Nth, _Last, std::less<>());
	}

		// FUNCTION TEMPLATE includes WITH PRED
template<class _InIt1,
	class _InIt2,
	class _Pr> inline
	bool includes(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _Pr _Pred)
	{	// test if every element in sorted [_First2, _Last2) is in sorted [_First1, _Last1), using _Pred
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	_DEBUG_ORDER_UNCHECKED(_UFirst1, _ULast1, _Pred);
	_DEBUG_ORDER_UNCHECKED(_UFirst2, _ULast2, _Pred);
	for (; _UFirst1 != _ULast1 && _UFirst2 != _ULast2; ++_UFirst1)
		{
		if (_DEBUG_LT_PRED(_Pred, *_UFirst2, *_UFirst1))
			{
			return (false);
			}

		if (!_Pred(*_UFirst1, *_UFirst2))
			{
			++_UFirst2;
			}
		}

	return (_UFirst2 == _ULast2);
	}

		// FUNCTION TEMPLATE includes
template<class _InIt1,
	class _InIt2> inline
	bool includes(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2)
	{	// test if every element in sorted [_First2, _Last2) is in sorted [_First1, _Last1), using operator<
	return (_STD includes(_First1, _Last1, _First2, _Last2, std::less<>()));
	}

		// FUNCTION TEMPLATE set_union WITH PRED
template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt _Set_union_no_deprecate(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _OutIt _Dest, _Pr _Pred)
	{	// OR sets [_First1, _Last1) and [_First2, _Last2), using _Pred, no deprecation warnings
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	_DEBUG_ORDER_UNCHECKED(_UFirst1, _ULast1, _Pred);
	_DEBUG_ORDER_UNCHECKED(_UFirst2, _ULast2, _Pred);
	auto _UDest = std::_Unchecked_idl0(_Dest);
	for (; _UFirst1 != _ULast1 && _UFirst2 != _ULast2; ++_UDest)
		{
		if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2))
			{	// copy first
			*_UDest = *_UFirst1;
			++_UFirst1;
			}
		else if (_Pred(*_UFirst2, *_UFirst1))
			{	// copy second
			*_UDest = *_UFirst2;
			++_UFirst2;
			}
		else
			{	// advance both
			*_UDest = *_UFirst1;
			++_UFirst1;
			++_UFirst2;
			}
		}

	_UDest = _Copy_no_deprecate(_UFirst1, _ULast1, _UDest);
	return (_Rechecked(_Dest, _Copy_no_deprecate(_UFirst2, _ULast2, _UDest)));
	}

template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt set_union(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _OutIt _Dest, _Pr _Pred)
	{	// OR sets [_First1, _Last1) and [_First2, _Last2), using _Pred
	_DEPRECATE_UNCHECKED(set_union, _Dest);
	return (_Set_union_no_deprecate(_First1, _Last1, _First2, _Last2, _Dest, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize,
	class _Pr> inline
	_DestTy *set_union(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _DestTy (&_Dest)[_DestSize], _Pr _Pred)
	{	// OR sets [_First1, _Last1) and [_First2, _Last2), array dest
	return (_Unchecked(
		_Set_union_no_deprecate(_First1, _Last1,
			_First2, _Last2,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE set_union
template<class _InIt1,
	class _InIt2,
	class _OutIt> inline
	_OutIt set_union(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _OutIt _Dest)
	{	// OR sets [_First1, _Last1) and [_First2, _Last2), using operator<
	return (_STD set_union(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize> inline
	_DestTy *set_union(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _DestTy (&_Dest)[_DestSize])
	{	// OR sets [_First1, _Last1) and [_First2, _Last2), array dest
	return (_STD set_union(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE set_intersection WITH PRED
template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt _Set_intersection_no_deprecate(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _OutIt _Dest, _Pr _Pred)
	{	// AND sets [_First1, _Last1) and [_First2, _Last2), using _Pred, no deprecation warnings
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	_DEBUG_ORDER_UNCHECKED(_UFirst1, _ULast1, _Pred);
	_DEBUG_ORDER_UNCHECKED(_UFirst2, _ULast2, _Pred);
	auto _UDest = std::_Unchecked_idl0(_Dest);
	while (_UFirst1 != _ULast1 && _UFirst2 != _ULast2)
		{
		if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2))
			{
			++_UFirst1;
			}
		else if (_Pred(*_UFirst2, *_UFirst1))
			{
			++_UFirst2;
			}
		else
			{
			*_UDest = *_UFirst1;
			++_UDest;
			++_UFirst1;
			++_UFirst2;
			}
		}

	return (_Rechecked(_Dest, _UDest));
	}

template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt set_intersection(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _OutIt _Dest, _Pr _Pred)
	{	// AND sets [_First1, _Last1) and [_First2, _Last2), using _Pred
	_DEPRECATE_UNCHECKED(set_intersection, _Dest);
	return (_Set_intersection_no_deprecate(_First1, _Last1, _First2, _Last2, _Dest, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize,
	class _Pr> inline
	_DestTy *set_intersection(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _DestTy (&_Dest)[_DestSize], _Pr _Pred)
	{	// AND sets [_First1, _Last1) and [_First2, _Last2), array dest
	return (_Unchecked(
		_Set_intersection_no_deprecate(_First1, _Last1,
			_First2, _Last2,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE set_intersection
template<class _InIt1,
	class _InIt2,
	class _OutIt> inline
	_OutIt set_intersection(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _OutIt _Dest)
	{	// AND sets [_First1, _Last1) and [_First2, _Last2), using operator<
	return (_STD set_intersection(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize> inline
	_DestTy *set_intersection(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _DestTy (&_Dest)[_DestSize])
	{	// AND sets [_First1, _Last1) and [_First2, _Last2), array dest
	return (_STD set_intersection(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE set_difference WITH PRED
template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt _Set_difference_no_deprecate(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest, _Pr _Pred)
	{	// take set [_First2, _Last2) from [_First1, _Last1), using _Pred, no deprecation warnings
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	_DEBUG_ORDER_UNCHECKED(_UFirst1, _ULast1, _Pred);
	_DEBUG_ORDER_UNCHECKED(_UFirst2, _ULast2, _Pred);
	auto _UDest = std::_Unchecked_idl0(_Dest);
	while (_UFirst1 != _ULast1 && _UFirst2 != _ULast2)
		{
		if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2))
			{	// copy first
			*_UDest = *_UFirst1;
			++_UDest;
			++_UFirst1;
			}
		else
			{
			if (!_Pred(*_UFirst2, *_UFirst1))
				{
				++_UFirst1;
				}

			++_UFirst2;
			}
		}

	return (_Rechecked(_Dest, _Copy_no_deprecate(_UFirst1, _ULast1, _UDest)));
	}

template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt set_difference(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest, _Pr _Pred)
	{	// take set [_First2, _Last2) from [_First1, _Last1), using _Pred
	_DEPRECATE_UNCHECKED(set_difference, _Dest);
	return (_Set_difference_no_deprecate(_First1, _Last1, _First2, _Last2, _Dest, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize,
	class _Pr> inline
	_DestTy *set_difference(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_DestTy (&_Dest)[_DestSize], _Pr _Pred)
	{	// take set [_First2, _Last2) from [_First1, _Last1), array dest
	return (_Unchecked(
		_Set_difference_no_deprecate(_First1, _Last1,
			_First2, _Last2,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE set_difference
template<class _InIt1,
	class _InIt2,
	class _OutIt> inline
	_OutIt set_difference(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest)
	{	// take set [_First2, _Last2) from [_First1, _Last1), using operator<
	return (_STD set_difference(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize> inline
	_DestTy *set_difference(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_DestTy (&_Dest)[_DestSize])
	{	// take set [_First2, _Last2) from [_First1, _Last1), array dest
	return (_STD set_difference(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE set_symmetric_difference WITH PRED
template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt _Set_symmetric_difference_no_deprecate(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest, _Pr _Pred)
	{	// XOR sets [_First1, _Last1) and [_First2, _Last2), using _Pred, no deprecation warnings
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_RANGE(_First2, _Last2);
	auto _UFirst1 = _Unchecked(_First1);
	const auto _ULast1 = _Unchecked(_Last1);
	auto _UFirst2 = _Unchecked(_First2);
	const auto _ULast2 = _Unchecked(_Last2);
	_DEBUG_ORDER_UNCHECKED(_UFirst1, _ULast1, _Pred);
	_DEBUG_ORDER_UNCHECKED(_UFirst2, _ULast2, _Pred);
	auto _UDest = std::_Unchecked_idl0(_Dest);
	while (_UFirst1 != _ULast1 && _UFirst2 != _ULast2)
		{
		if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2))
			{	// copy first
			*_UDest = *_UFirst1;
			++_UDest;
			++_UFirst1;
			}
		else if (_Pred(*_UFirst2, *_UFirst1))
			{	// copy second
			*_UDest = *_UFirst2;
			++_UDest;
			++_UFirst2;
			}
		else
			{	// advance both
			++_UFirst1;
			++_UFirst2;
			}
		}

	_UDest = _Copy_no_deprecate(_UFirst1, _ULast1, _UDest);
	return (_Rechecked(_Dest, _Copy_no_deprecate(_UFirst2, _ULast2, _UDest)));
	}

template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Pr> inline
	_OutIt set_symmetric_difference(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest, _Pr _Pred)
	{	// XOR sets [_First1, _Last1) and [_First2, _Last2), using _Pred
	_DEPRECATE_UNCHECKED(set_symmetric_difference, _Dest);
	return (_Set_symmetric_difference_no_deprecate(
		_First1, _Last1, _First2, _Last2, _Dest, _Pass_fn(_Pred)));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize,
	class _Pr> inline
	_DestTy *set_symmetric_difference(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_DestTy (&_Dest)[_DestSize], _Pr _Pred)
	{	// XOR sets [_First1, _Last1) and [_First2, _Last2), array dest
	return (_Unchecked(
		_Set_symmetric_difference_no_deprecate(_First1, _Last1,
			_First2, _Last2,
			_Array_iterator<_DestTy, _DestSize>(_Dest), _Pass_fn(_Pred))));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE set_symmetric_difference
template<class _InIt1,
	class _InIt2,
	class _OutIt> inline
	_OutIt set_symmetric_difference(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest)
	{	// XOR sets [_First1, _Last1) and [_First2, _Last2), using operator<
	return (_STD set_symmetric_difference(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}

 #if _ITERATOR_DEBUG_ARRAY_OVERLOADS
template<class _InIt1,
	class _InIt2,
	class _DestTy,
	size_t _DestSize> inline
	_DestTy *set_symmetric_difference(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_DestTy (&_Dest)[_DestSize])
	{	// XOR sets [_First1, _Last1) and [_First2, _Last2), array dest
	return (_STD set_symmetric_difference(_First1, _Last1, _First2, _Last2, _Dest, std::less<>()));
	}
 #endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

		// FUNCTION TEMPLATE max_element WITH PRED
template<class _FwdIt,
	class _Pr>
	constexpr _FwdIt _Max_element_unchecked(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find largest element, using _Pred
	_FwdIt _Found = _First;
	if (_First != _Last)
		{
		while (++_First != _Last)
			{
			if (_DEBUG_LT_PRED(_Pred, *_Found, *_First))
				{
				_Found = _First;
				}
			}
		}

	return (_Found);
	}

template<class _FwdIt,
	class _Pr>
	constexpr _FwdIt max_element(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find largest element, using _Pred
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Max_element_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred))));
	}

		// FUNCTION TEMPLATE max_element
template<class _FwdIt>
	constexpr _FwdIt max_element(_FwdIt _First, _FwdIt _Last)
	{	// find largest element, using operator<
	return (_STD max_element(_First, _Last, std::less<>()));
	}

		// FUNCTION TEMPLATE min_element WITH PRED
template<class _FwdIt,
	class _Pr>
	constexpr _FwdIt _Min_element_unchecked(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find smallest element, using _Pred
	_FwdIt _Found = _First;
	if (_First != _Last)
		{
		while (++_First != _Last)
			{
			if (_DEBUG_LT_PRED(_Pred, *_First, *_Found))
				{
				_Found = _First;
				}
			}
		}

	return (_Found);
	}

template<class _FwdIt,
	class _Pr>
	constexpr _FwdIt min_element(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find smallest element, using _Pred
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Min_element_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred))));
	}

		// FUNCTION TEMPLATE min_element
template<class _FwdIt>
	constexpr _FwdIt min_element(_FwdIt _First, _FwdIt _Last)
	{	// find smallest element, using operator<
	return (_STD min_element(_First, _Last, std::less<>()));
	}

		// FUNCTION TEMPLATE minmax_element WITH PRED
template<class _FwdIt,
	class _Pr>
	constexpr std::pair<_FwdIt, _FwdIt> _Minmax_element_unchecked(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find smallest and largest elements, using _Pred
	std::pair<_FwdIt, _FwdIt> _Found(_First, _First);

	if (_First != _Last)
		{
		while (++_First != _Last)
			{	// process one or two elements
			_FwdIt _Next = _First;
			if (++_Next == _Last)
				{	// process last element
				if (_DEBUG_LT_PRED(_Pred, *_First, *_Found.first))
					{
					_Found.first = _First;
					}
				else if (!_DEBUG_LT_PRED(_Pred, *_First, *_Found.second))
					{
					_Found.second = _First;
					}
				}
			else
				{	// process next two elements
				if (_DEBUG_LT_PRED(_Pred, *_Next, *_First))
					{	// test _Next for new smallest
					if (_DEBUG_LT_PRED(_Pred, *_Next, *_Found.first))
						{
						_Found.first = _Next;
						}
					if (!_DEBUG_LT_PRED(_Pred, *_First, *_Found.second))
						{
						_Found.second = _First;
						}
					}
				else
					{	// test _First for new smallest
					if (_DEBUG_LT_PRED(_Pred, *_First, *_Found.first))
						{
						_Found.first = _First;
						}
					if (!_DEBUG_LT_PRED(_Pred, *_Next, *_Found.second))
						{
						_Found.second = _Next;
						}
					}
				_First = _Next;
				}
			}
		}

	return (_Found);
	}

template<class _FwdIt,
	class _Pr>
	constexpr std::pair<_FwdIt, _FwdIt> minmax_element(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find smallest and largest elements, using _Pred
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked_both(_First, _Last,
		_Minmax_element_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred))));
	}

		// FUNCTION TEMPLATE minmax_element
template<class _FwdIt>
	constexpr std::pair<_FwdIt, _FwdIt> minmax_element(_FwdIt _First, _FwdIt _Last)
	{	// find smallest and largest elements, using operator<
	return (_STD minmax_element(_First, _Last, std::less<>()));
	}

		// FUNCTION TEMPLATE max WITH PRED
template<class _Ty,
	class _Pr>
	constexpr const _Ty& (max)(const _Ty& _Left, const _Ty& _Right,
		_Pr _Pred)
	{	// return larger of _Left and _Right using _Pred
	return (_DEBUG_LT_PRED(_Pred, _Left, _Right) ? _Right : _Left);
	}

template<class _Ty,
	class _Pr>
	constexpr _Ty (max)(initializer_list<_Ty> _Ilist, _Pr _Pred)
	{	// return leftmost/largest
	const _Ty *_Res = _Max_element_unchecked(_Ilist.begin(), _Ilist.end(), _Pass_fn(_Pred));
	return (*_Res);
	}

		// FUNCTION TEMPLATE max
#pragma warning(push)
#pragma warning(disable: 28285)	// (syntax error in SAL annotation, occurs when _Ty is not an integral type)
template<class _Ty>
	_Post_equal_to_(_Left < _Right ? _Right : _Left)
	constexpr const _Ty& (max)(const _Ty& _Left, const _Ty& _Right)
		_NOEXCEPT_COND(_NOEXCEPT_OPER(_DEBUG_LT(_Left, _Right)))
	{	// return larger of _Left and _Right
	return (_DEBUG_LT(_Left, _Right) ? _Right : _Left);
	}
#pragma warning(pop)

template<class _Ty>
	constexpr _Ty (max)(initializer_list<_Ty> _Ilist)
	{	// return leftmost/largest
	return ((_STD max)(_Ilist, std::less<>()));
	}

		// FUNCTION TEMPLATE min WITH PRED
template<class _Ty,
	class _Pr>
	constexpr const _Ty& (min)(const _Ty& _Left, const _Ty& _Right,
		_Pr _Pred)
		_NOEXCEPT_COND(_NOEXCEPT_OPER(_DEBUG_LT_PRED(_Pred, _Right, _Left)))
	{	// return smaller of _Left and _Right using _Pred
	return (_DEBUG_LT_PRED(_Pred, _Right, _Left) ? _Right : _Left);
	}

template<class _Ty,
	class _Pr>
	constexpr _Ty (min)(initializer_list<_Ty> _Ilist, _Pr _Pred)
	{	// return leftmost/smallest
	const _Ty *_Res = _Min_element_unchecked(_Ilist.begin(), _Ilist.end(), _Pass_fn(_Pred));
	return (*_Res);
	}

		// FUNCTION TEMPLATE min
#pragma warning(push)
#pragma warning(disable: 28285)	// (syntax error in SAL annotation, occurs when _Ty is not an integral type)
template<class _Ty>
	_Post_equal_to_(_Right < _Left ? _Right : _Left)
	constexpr const _Ty& (min)(const _Ty& _Left, const _Ty& _Right)
		_NOEXCEPT_COND(_NOEXCEPT_OPER(_DEBUG_LT(_Right, _Left)))
	{	// return smaller of _Left and _Right
	return (_DEBUG_LT(_Right, _Left) ? _Right : _Left);
	}
#pragma warning(pop)

template<class _Ty>
	constexpr _Ty (min)(initializer_list<_Ty> _Ilist)
	{	// return leftmost/smallest
	return ((_STD min)(_Ilist, std::less<>()));
	}


		// FUNCTION TEMPLATE minmax WITH PRED
template<class _Ty,
	class _Pr>
	constexpr std::pair<const _Ty&, const _Ty&>
		minmax(const _Ty& _Left, const _Ty& _Right, _Pr _Pred)
	{	// return std::pair(leftmost/smaller, rightmost/larger) of _Left and _Right
	return (_Pred(_Right, _Left)
		? std::pair<const _Ty&, const _Ty&>(_Right, _Left)
		: std::pair<const _Ty&, const _Ty&>(_Left, _Right));
	}

template<class _Ty,
	class _Pr>
	constexpr std::pair<_Ty, _Ty> minmax(initializer_list<_Ty> _Ilist,
		_Pr _Pred)
	{	// return {leftmost/smallest, rightmost/largest}
	std::pair<const _Ty *, const _Ty *> _Res = _Minmax_element_unchecked(
		_Ilist.begin(), _Ilist.end(), _Pass_fn(_Pred));
	return (std::pair<_Ty, _Ty>(*_Res.first, *_Res.second));
	}

		// FUNCTION TEMPLATE minmax
template<class _Ty>
	constexpr std::pair<const _Ty&, const _Ty&>
		minmax(const _Ty& _Left, const _Ty& _Right)
	{	// return std::pair(leftmost/smaller, rightmost/larger) of _Left and _Right
	return (_Right < _Left
		? std::pair<const _Ty&, const _Ty&>(_Right, _Left)
		: std::pair<const _Ty&, const _Ty&>(_Left, _Right));
	}

template<class _Ty>
	constexpr std::pair<_Ty, _Ty> minmax(initializer_list<_Ty> _Ilist)
	{	// return {leftmost/smallest, rightmost/largest}
	return (_STD minmax(_Ilist, std::less<>()));
	}

		// FUNCTION TEMPLATE next_permutation WITH PRED
template<class _BidIt,
	class _Pr> inline
	bool next_permutation(_BidIt _First, _BidIt _Last, _Pr _Pred)
	{	// permute and test for pure ascending, using _Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UNext = _ULast;
	if (_UFirst == _ULast || _UFirst == --_UNext)
		{
		return (false);
		}

	for (;;)
		{	// find rightmost element smaller than successor
		auto _UNext1 = _UNext;
		if (_DEBUG_LT_PRED(_Pred, *--_UNext, *_UNext1))
			{	// swap with rightmost element that's smaller, flip suffix
			auto _UMid = _ULast;
			do
				{
				--_UMid;
				}
			while (!_DEBUG_LT_PRED(_Pred, *_UNext, *_UMid));

			_STD iter_swap(_UNext, _UMid);
			_Reverse_unchecked(_UNext1, _ULast);
			return (true);
			}

		if (_UNext == _UFirst)
			{	// pure descending, flip all
			_Reverse_unchecked(_UFirst, _ULast);
			return (false);
			}
		}
	}

		// FUNCTION TEMPLATE next_permutation
template<class _BidIt> inline
	bool next_permutation(_BidIt _First, _BidIt _Last)
	{	// permute and test for pure ascending, using operator<
	return (_STD next_permutation(_First, _Last, std::less<>()));
	}

		// FUNCTION TEMPLATE prev_permutation WITH PRED
template<class _BidIt,
	class _Pr> inline
	bool prev_permutation(_BidIt _First, _BidIt _Last, _Pr _Pred)
	{	// reverse permute and test for pure descending, using _Pred
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UNext = _ULast;
	if (_UFirst == _ULast || _UFirst == --_UNext)
		{
		return (false);
		}

	for (;;)
		{	// find rightmost element not smaller than successor
		auto _UNext1 = _UNext;
		if (_DEBUG_LT_PRED(_Pred, *_UNext1, *--_UNext))
			{	// swap with rightmost element that's not smaller, flip suffix
			auto _UMid = _ULast;
			do
				{
				--_UMid;
				}
			while (!_DEBUG_LT_PRED(_Pred, *_UMid, *_UNext));

			_STD iter_swap(_UNext, _UMid);
			_Reverse_unchecked(_UNext1, _ULast);
			return (true);
			}

		if (_UNext == _UFirst)
			{	// pure ascending, flip all
			_Reverse_unchecked(_UFirst, _ULast);
			return (false);
			}
		}
	}

		// FUNCTION TEMPLATE prev_permutation
template<class _BidIt> inline
	bool prev_permutation(_BidIt _First, _BidIt _Last)
	{	// reverse permute and test for pure descending, using operator<
	return (_STD prev_permutation(_First, _Last, std::less<>()));
	}

		// FUNCTION TEMPLATES is_sorted AND is_sorted_until WITH PRED
template<class _FwdIt,
	class _Pr> inline
	_FwdIt _Is_sorted_until_unchecked(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find extent of range that is ordered by predicate
	if (_First != _Last)
		{
		for (_FwdIt _Next = _First; ++_Next != _Last; ++_First)
			{
			if (_DEBUG_LT_PRED(_Pred, *_Next, *_First))
				{
				return (_Next);
				}
			}
		}

	return (_Last);
	}

template<class _FwdIt,
	class _Pr> inline
	_FwdIt is_sorted_until(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// find extent of range that is ordered by predicate
	_DEBUG_RANGE(_First, _Last);
	return (_Rechecked(_First,
		_Is_sorted_until_unchecked(_Unchecked(_First), _Unchecked(_Last), _Pass_fn(_Pred))));
	}

template<class _FwdIt,
	class _Pr> inline
	bool is_sorted(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
	{	// test if range is ordered by predicate
	_DEBUG_RANGE(_First, _Last);
	const auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	return (_Is_sorted_until_unchecked(_UFirst, _ULast, _Pass_fn(_Pred)) == _ULast);
	}

		// FUNCTION TEMPLATES is_sorted AND is_sorted_until
template<class _FwdIt> inline
	_FwdIt is_sorted_until(_FwdIt _First, _FwdIt _Last)
	{	// find extent of range that is ordered by operator<
	return (_STD is_sorted_until(_First, _Last, std::less<>()));
	}

template<class _FwdIt> inline
	bool is_sorted(_FwdIt _First, _FwdIt _Last)
	{	// test if range is ordered by operator<
	return (_STD is_sorted(_First, _Last, std::less<>()));
	}

} // namespace detail
} // namespace jsoncons

#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)

#endif // JSONCONS_DETAIL_STABLE_SORT

