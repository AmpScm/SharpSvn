#pragma once

namespace SharpGit {
	namespace Diff {

		generic<typename T> where T : System::IComparable<T>, ref class
		public ref class DiffPair
		{
		private:
			initonly T _left;
			initonly T _right;

		public:
			DiffPair(T left, T right)
			{
				_left = left;
				_right = right;
			}

			property T Left
			{
				T get()
				{
					return _left;
				}
			}

			property T Right
			{
				T get()
				{
					return _right;
				}
			}

			property bool HasLeft
			{
				bool get()
				{
					return (Object^)_left != nullptr;
				}
			}

			property bool HasRight
			{
				bool get()
				{
					return (Object^)_left != nullptr;
				}
			}
		};

		generic<typename T> where T : System::IComparable<T>, ref class
		public ref class DiffTripple : public DiffPair<T>
		{
		private:
			initonly T _base;

		public:
			DiffTripple(T left, T right, T base)
				: DiffPair(left, right)
			{
				_base = base;
			}

			property T Base
			{
				T get()
				{
					return _base;
				}
			}

			property bool HasBase
			{
				bool get()
				{
					return (Object^)_base != nullptr;
				}
			}
		};

		generic<typename T> where T : System::IComparable<T>, ref class
		public ref class DiffEnumerable sealed : public System::Collections::Generic::IEnumerable<DiffPair<T>^>
		{
		private:
			initonly IEnumerable<T>^ _left;
			initonly IEnumerable<T>^ _right;
			initonly System::Collections::Generic::Comparer<T>^ _comparer;

		public:
			DiffEnumerable(IEnumerable<T>^ left, IEnumerable<T>^ right)
			{
				if (! left)
					throw gcnew ArgumentNullException("left");
				else if (! right)
					throw gcnew ArgumentNullException("right");

				_left = left;
				_right = right;
				_comparer = System::Collections::Generic::Comparer<T>::Default;
			}

			DiffEnumerable(IEnumerable<T>^ left, IEnumerable<T>^ right, System::Collections::Generic::Comparer<T>^ comparer)
			{
				if (! left)
					throw gcnew ArgumentNullException("left");
				else if (! right)
					throw gcnew ArgumentNullException("right");
				else if (! comparer)
					throw gcnew ArgumentNullException("comparer");

				_left = left;
				_right = right;
				_comparer = comparer;
			}

		private:
			virtual System::Collections::Generic::IEnumerator<DiffPair<T>^>^ GetGenericEnumerator() sealed
				= System::Collections::Generic::IEnumerable<DiffPair<T>^>::GetEnumerator
			{
				return GetEnumerator();
			}

			virtual System::Collections::IEnumerator^ GetObjectEnumerator() sealed
				= System::Collections::IEnumerable::GetEnumerator
			{
				return GetEnumerator();
			}

		public:
			ref class DiffEnumerator sealed : public System::Collections::Generic::IEnumerator<DiffPair<T>^>
			{
			private:
				initonly IEnumerator<T>^ _left;
				initonly IEnumerator<T>^ _right;
				initonly System::Collections::Generic::Comparer<T>^ _comparer;
				bool _haveLeft;
				bool _haveRight;
				DiffPair<T>^ _current;

			private:
				~DiffEnumerator()
				{
					delete _left;
					delete _right;
				}

			public:
				DiffEnumerator(IEnumerable<T>^ left, IEnumerable<T>^ right, System::Collections::Generic::Comparer<T>^ comparer)
				{
					if (! left)
						throw gcnew ArgumentNullException("left");
					else if (! right)
					throw gcnew ArgumentNullException("right");

					_left = left->GetEnumerator();
					_right = right->GetEnumerator();
					_comparer = comparer;
				}

				property DiffPair<T>^ Current
				{
					virtual DiffPair<T>^ get()
					{
						return _current;
					}
				}

				virtual bool MoveNext()
				{
					_current = nullptr;

					if (!_haveLeft)
						_haveLeft = _left->MoveNext();
					if (!_haveRight)
						_haveRight = _right->MoveNext();

					if (! _haveLeft)
					{
						if (_haveRight)
						{
							_haveRight = false;
							_current = gcnew DiffPair<T>(T(), _right->Current);
						}
						else
							return false;
					}
					else if (! _haveRight)
					{
						_haveLeft = false;
						_current = gcnew DiffPair<T>(_left->Current, T());
					}
					else
					{
						T lv = _left->Current;
						T rv = _right->Current;

						int n = _comparer->Compare(lv, rv);

						if (n == 0)
						{
							_current = gcnew DiffPair<T>(lv, rv);
							_haveLeft = false;
							_haveRight = false;
						}
						else if (n < 0)
						{
							_current = gcnew DiffPair<T>(lv, T());
							_haveLeft = false;
						}
						else
						{
							_current = gcnew DiffPair<T>(T(), rv);
							_haveRight = false;
						}
					}
					return true;
				}

				virtual void Reset()
				{
					_left->Reset();
					_right->Reset();
					_haveLeft = false;
					_haveRight = false;
					_current = nullptr;
				}

			private:
				property Object^ ObjectCurrent
				{
					virtual Object^ get() sealed = System::Collections::IEnumerator::Current::get
					{
						return Current;
					}
				}

			};

		public:
			DiffEnumerator^ GetEnumerator()
			{
				return gcnew DiffEnumerator(_left, _right, _comparer);
			}
		};

		generic<typename T> where T : System::IComparable<T>, ref class
		public ref class DiffBaseEnumerable : public System::Collections::Generic::IEnumerable<DiffTripple<T>^>
		{
		private:
			initonly IEnumerable<T>^ _base;
			initonly IEnumerable<T>^ _left;
			initonly IEnumerable<T>^ _right;
			initonly System::Collections::Generic::Comparer<T>^ _comparer;

		public:
			DiffBaseEnumerable(IEnumerable<T>^ base, IEnumerable<T>^ left, IEnumerable<T>^ right)
			{
				if (! base)
					throw gcnew ArgumentNullException("base");
				else if (! left)
					throw gcnew ArgumentNullException("left");
				else if (! right)
					throw gcnew ArgumentNullException("right");

				_base = base;
				_left = left;
				_right = right;
				_comparer = System::Collections::Generic::Comparer<T>::Default;
			}

			DiffBaseEnumerable(IEnumerable<T>^ base, IEnumerable<T>^ left, IEnumerable<T>^ right, System::Collections::Generic::Comparer<T>^ comparer)
			{
				if (! base)
					throw gcnew ArgumentNullException("base");
				else if (! left)
					throw gcnew ArgumentNullException("left");
				else if (! right)
					throw gcnew ArgumentNullException("right");
				else if (! comparer)
					throw gcnew ArgumentNullException("comparer");

				_base = base;
				_left = left;
				_right = right;
				_comparer = comparer;
			}

		private:
			virtual System::Collections::Generic::IEnumerator<DiffTripple<T>^>^ GetGenericEnumerator() sealed
				= System::Collections::Generic::IEnumerable<DiffTripple<T>^>::GetEnumerator
			{
				return GetEnumerator();
			}

			virtual System::Collections::IEnumerator^ GetObjectEnumerator() sealed
				= System::Collections::IEnumerable::GetEnumerator
			{
				return GetEnumerator();
			}

		public:
			ref class DiffBaseEnumerator sealed : public System::Collections::Generic::IEnumerator<DiffTripple<T>^>
			{
			private:
				initonly IEnumerator<T>^ _left;
				initonly IEnumerator<T>^ _right;
				initonly IEnumerator<T>^ _base;
				initonly System::Collections::Generic::Comparer<T>^ _comparer;
				bool _haveLeft;
				bool _haveRight;
				bool _haveBase;
				DiffTripple<T>^ _current;

			private:
				~DiffBaseEnumerator()
				{
					delete _left;
					delete _right;
					delete _base;
				}

			public:
				DiffBaseEnumerator(IEnumerable<T>^ left, IEnumerable<T>^ right, IEnumerable<T>^ base, System::Collections::Generic::Comparer<T>^ comparer)
				{
					if (! left)
						throw gcnew ArgumentNullException("left");
					else if (! right)
					throw gcnew ArgumentNullException("right");

					_left = left->GetEnumerator();
					_right = right->GetEnumerator();
					_base = base->GetEnumerator();
					_comparer = comparer;
				}

				property DiffTripple<T>^ Current
				{
					virtual DiffTripple<T>^ get()
					{
						return _current;
					}
				}

				virtual bool MoveNext()
				{
					_current = nullptr;

					if (!_haveLeft)
						_haveLeft = _left->MoveNext();
					if (!_haveRight)
						_haveRight = _right->MoveNext();
					if (!_haveBase)
						_haveBase = _base->MoveNext();

					// NOTE: T() is a special C++/CLI syntax for default value, which is
					//       null for reference classes.
					T lv;
					T rv;
					T bv;

					if (_haveLeft)
					{
						lv = _left->Current;

						if (_haveRight)
						{
							rv = _right->Current;

							int lr = _comparer->Compare(lv, rv);

							if (lr <= 0)
							{
								// left <= right

								if (lr < 0)
									rv = T(); /* We can ignore right */

								if (_haveBase)
								{
									bv = _base->Current;

									int lb = _comparer->Compare(lv, bv);

									if (lb < 0)
									{
										_haveLeft = false;
										if (lr == 0)
											_haveRight = false;

										_current = gcnew DiffTripple<T>(lv, rv, T());
									}
									else if (lb > 0)
									{
										_haveBase = false;
										_current = gcnew DiffTripple<T>(T(), T(), bv);
									}
									else
									{
										_haveLeft = false;
										_haveBase = false;
										if (lr == 0)
											_haveRight = false;
										_current = gcnew DiffTripple<T>(lv, rv, bv);
									}
								}
								else
								{
									_haveLeft = false;
									if (lr == 0)
										_haveRight = false;
									_current = gcnew DiffTripple<T>(lv, rv, T());
								}
							}
							else
							{
								// right < left

								if (_haveBase)
								{
									bv = _base->Current;

									int rb = _comparer->Compare(rv, bv);

									if (rb <= 0)
										_haveRight = false;
									else
										rv = T();

									if (rb >= 0)
										_haveBase = false;
									else
										bv = T();

									_current = gcnew DiffTripple<T>(T(), rv, bv);
								}
								else
								{
									_haveRight = false;
									_current = gcnew DiffTripple<T>(T(), rv, T());
								}
							}
						}
						else if (_haveBase)
						{
							// Have left and base
							bv = _base->Current;

							int lb = _comparer->Compare(lv, bv);

							if (lb <= 0)
								_haveLeft = false;
							else
								lv = T();

							if (lb >= 0)
								_haveBase = false;
							else
								bv = T();

							_current = gcnew DiffTripple<T>(lv, T(), bv);
						}
						else
						{
							_haveLeft = false;
							_current = gcnew DiffTripple<T>(lv, T(), T());
						}
					}
					else if (_haveRight)
					{
						rv = _right->Current;

						if (_haveBase)
						{
							bv = _base->Current;

							int rb = _comparer->Compare(rv, bv);

							if (rb <= 0)
								_haveRight = false;
							else
								rv = T();

							if (rb >= 0)
								_haveBase = false;
							else
								bv = T();

							_current = gcnew DiffTripple<T>(T(), rv, bv);
						}
						else
						{
							_haveRight = false;
							_current = gcnew DiffTripple<T>(T(), rv, T());
						}
					}
					else if (_haveBase)
					{
						// no left or right
						_haveBase = false;
						_current = gcnew DiffTripple<T>(T(), T(), _base->Current);
					}
					else
						return false;

					return true;
				}

				virtual void Reset()
				{
					_left->Reset();
					_right->Reset();
					_haveLeft = false;
					_haveRight = false;
					_current = nullptr;
				}

			private:
				property Object^ ObjectCurrent
				{
					virtual Object^ get() sealed = System::Collections::IEnumerator::Current::get
					{
						return Current;
					}
				}

			};

		public:
			DiffBaseEnumerator^ GetEnumerator()
			{
				return gcnew DiffBaseEnumerator(_base, _left, _right, _comparer);
			}
		};

	}
}
