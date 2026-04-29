#include <BitcoinExchange.hpp>

BitcoinExchange::BitcoinExchange() : data_file("data.csv")
{
	this->processingData();
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other)
{
	this->data = other.data;
	this->input = other.input;
}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this == &other)
		return (*this);
	this->data = other.data;
	this->input = other.input;
	return (*this);
}

BitcoinExchange::~BitcoinExchange()
{
	this->data_file.close();
}
void BitcoinExchange::processingData()
{
		double num;

	std::string line;
	std::getline(data_file, line);
	while (std::getline(data_file, line))
	{
		std::stringstream ss(line.substr(line.find(',') + 1));
		ss >> num;
		this->data.insert(std::make_pair(line.substr(0, line.find(',')), num));
	}
}
void BitcoinExchange::exchange(std::ifstream &input)
{
	double	value;

	std::string line;
	if (input.eof() || !std::getline(input, line)
		|| line.compare("date | value"))
		throw std::runtime_error("Invalid file");
	while (std::getline(input, line))
	{
		if (line.empty())
			continue ;
		if (line.size() < 14 || !(line[10] == ' ' && line[11] == '|'
				&& line[12] == ' '))
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue ;
		}
		std::string date_str = line.substr(0, 10);
		if (!valid_date(date_str))
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue ;
		}
		value = valid_value(line.substr(13));
		if (value < 0)
			continue ;
		std::map<std::string,
			double>::iterator found = this->data.find(date_str);
		if (found == data.end())
		{
			found = this->data.lower_bound(date_str);
			if (found == data.begin())
			{
				std::cout << "Error: price wasn't found" << std::endl;
				continue ;
			}
			--found;
		}
		std::cout << date_str << " => " << value << " = " << value
			* found->second << std::endl;
	}
}

bool BitcoinExchange::valid_date(std::string date)
{
	if (date.size() != 10)
		return (false);
	int year, month, day;
	char dash1, dash2;
	std::stringstream ss(date);
	ss >> year >> dash1 >> month >> dash2 >> day;
	if (ss.fail() || dash1 != '-' || dash2 != '-')
		return (false);
	if (year < 2009 || year > 2022 || month < 1 || month > 12 || day < 1
		|| day > 31)
		return (false);
	switch (month)
	{
	case 2:
		return (day <= (year % 4 == 0 ? 29 : 28));
	case 4:
	case 6:
	case 9:
	case 11:
		return (day <= 30);
	}
	return (true);
}

double BitcoinExchange::valid_value(std::string value_s)
{
	double	value;

	std::stringstream ss(value_s);
	ss >> value;
	if (ss.str().find_first_not_of("-0123456789.") != std::string::npos
		|| ss.str().find(".") != ss.str().rfind(".")
		|| (ss.str().rfind("-") != std::string::npos
			&& ss.str().rfind("-") != 0) || ss.fail() || value < 0
		|| value > 1000)
	{
		if (value < 0)
			std::cout << "Error: not a positive number." << std::endl;
		else if (value > 1000)
			std::cout << "Error: too large a number." << std::endl;
		else
			std::cout << "Error: invalid value format => " << ss.str() << std::endl;
		return (-1);
	}
	return (value);
}
