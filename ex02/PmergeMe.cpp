#include <PmergeMe.hpp>

PmergeMe::PmergeMe()
{
	std::cout<<"PmergeMe default ctor called!\n";
}

PmergeMe::PmergeMe(const PmergeMe &other)
{
	this->arr_d = other.arr_d;
	this->arr_v = other.arr_v;
	std::cout<<"PmergeMe copy ctor called!\n";
}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
	if (this == &other)
		return (*this);
	this->arr_d = other.arr_d;
	this->arr_v = other.arr_v;
	std::cout<<"PmergeMe copy assignment operator called!\n";
	return (*this);
}

PmergeMe::~PmergeMe()
{
	std::cout<<"PmergeMe dtor called!\n";
}

void PmergeMe::fill(int argc, char *argv[])
{
	long	ii;

	for (int i = 1; i < argc; i++)
	{
		std::stringstream ss(argv[i]);
		if (ss.str().size() > 10
			|| ss.str().find_first_not_of("+0123456789") != std::string::npos
			|| (ss.str().find('+') != std::string::npos
				&& ss.str().find_last_of('+') != 0))
			throw std::runtime_error("Invalid input");
		ss >> ii;
		if (ii > 2147483647 || ii < 0)
			throw std::runtime_error("Number is bigger than INT_MAX");
		arr_v.push_back(ii);
		arr_d.push_back(ii);
	}
}
void PmergeMe::print()
{
	for (size_t i = 0; i < arr_v.size(); i++)
		std::cout << " " << arr_v[i];
	std::cout << std::endl;
}

static std::vector<int> getJacobsthalSequence(size_t size)
{
	int	prev;
	int	curr;
	int	next;

	std::vector<int> jacobsthal;
	if (size == 0)
		return (jacobsthal);
	prev = 1;
	curr = 3;
	jacobsthal.push_back(1);
	if (size > 1)
		jacobsthal.push_back(3);
	while (static_cast<int>(jacobsthal.back()) < static_cast<int>(size))
	{
		next = curr * 2 + prev;
		jacobsthal.push_back(next);
		prev = curr;
		curr = next;
	}
	return (jacobsthal);
}

template <typename T> void binaryInsert(T &big, typename T::value_type val)
{
	typename T::iterator left = big.begin();
	typename T::iterator right = big.end();
	while (left < right)
	{
		typename T::iterator mid = left + std::distance(left, right) / 2;
		if (val <= *mid)
			right = mid;
		else
			left = mid + 1;
	}
	big.insert(left, val);
}

template <typename T> T mergeInsertSort(T arr)
{
	bool	hasUnpaired;
	T		big;
	T		small;
	int		currIdx;
	int		prevIdx;

	if (arr.size() <= 1)
		return (arr);
	typename std::vector<std::pair<typename T::value_type,
		typename T::value_type> > pairs;
	typename T::value_type unpaired;
	hasUnpaired = false;
	for (size_t i = 0; i + 1 < arr.size(); i += 2)
	{
		if (arr[i] > arr[i + 1])
			pairs.push_back(std::make_pair(arr[i], arr[i + 1]));
		else
			pairs.push_back(std::make_pair(arr[i + 1], arr[i]));
	}
	if (arr.size() % 2 == 1)
	{
		unpaired = arr[arr.size() - 1];
		hasUnpaired = true;
	}
	for (size_t i = 0; i < pairs.size(); i++)
	{
		big.push_back(pairs[i].first);
		small.push_back(pairs[i].second);
	}
	big = mergeInsertSort(big);
	std::vector<int> jacob = getJacobsthalSequence(small.size());
	std::vector<bool> inserted(small.size(), false);
	for (size_t j = 0; j < jacob.size(); j++)
	{
		currIdx = jacob[j] - 1;
		prevIdx = (j > 0) ? jacob[j - 1] - 1 : -1;
		while (currIdx > prevIdx)
		{
			if (static_cast<size_t>(currIdx) < small.size()
				&& !inserted[currIdx])
			{
				binaryInsert(big, small[currIdx]);
				inserted[currIdx] = true;
			}
			currIdx--;
		}
	}
	for (size_t i = 0; i < small.size(); i++)
	{
		if (!inserted[i])
			binaryInsert(big, small[i]);
	}
	if (hasUnpaired)
		binaryInsert(big, unpaired);
	return (big);
}

void PmergeMe::sort(int argc, char *argv[])
{
	double	deque_time;
	double	vector_time;

	clock_t start, end;
	this->fill(argc, argv);
	std::cout << "Before: ";
	this->print();
	start = clock();
	this->arr_v = mergeInsertSort(this->arr_v);
	end = clock();
	vector_time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000;
	std::cout << "After: ";
	this->print();
	start = clock();
	this->arr_d = mergeInsertSort(this->arr_d);
	end = clock();
	deque_time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000;
	std::cout << "Time to process a range of " << this->arr_v.size() << " elements with std::vector : " << vector_time << " us" << std::endl;
	std::cout << "Time to process a range of " << this->arr_d.size() << " elements with std::deque : " << deque_time << " us" << std::endl;
}
