#include<vector>
#include<iostream>
#include<sstream>
#include<valarray>
#include<string>
#include<type_traits>
#include<exception>
using namespace std;
#define KELVIN -1
#define CELSIUS 0
#define FAHRENHEIT 1
#define COMMERCIAL -100
#define MILITARY -101
#define INDUSTRIAL -102
#define AUTOMOTIVE -103
#define EXTENDED -104
#define MIN -4
#define MAX 4
#define KELVIN_SPEC "^K"
#define CELSIUS_SPEC "^C"
#define FAHRENHEIT_SPEC "^F"

/*
* Exception class for temperature class
*/
class Temp_Exception : public exception {
private:
	string excep;
public:
	Temp_Exception(string ex) : excep{ ex } {}
	const char* what() const noexcept { return excep.c_str(); }
	~Temp_Exception() { excep.clear(); }
};


template <typename T>
class Temperature {
	static_assert(is_floating_point<T>::value, "Error: Temperature require floating poing type");
private:
	//enumeration for ranges in celsius
	enum class Ranges_Celsius : int {
		lower_military = -55,
		lower_intdustrial = -40,
		lower_automotive = -40,
		lower_extended = -40,
		lower_commercial = 0,
		upper_commercial  = 85,
		upper_military = 125,
		upper_industrial = 100,
		upper_automotive = 125,
		upper_exetended = 125
	};
	//enumeration for ranges name index
	enum class Ranges_Index : int {
		commercial =0,
		military,
		industrial,
		extended,
		automotive,
		not_in_range
	};

	vector<string> ranges_names{ "commercial","military","industrial","extended","automotive","not in range" };
	string def_spec;
	valarray<T> temp;
	int temp_type{CELSIUS};
	bool is_kelvin{ false };
	bool is_celsius{ false };
	bool is_fahrenheit{ false };

	void append_range(string&, string);
	bool is_range(Temperature, int = COMMERCIAL);
	void set_flags_zero();
public:
	Temperature() = default;
	Temperature(valarray<T>, int = CELSIUS);
	Temperature(int);
	Temperature(const Temperature&) = default;
	Temperature& operator=(const Temperature&) = default;
	Temperature(Temperature&&) = default;
	Temperature& operator=(Temperature&&) = default;
	virtual ~Temperature();


	Temperature to_Celsius(); 
	Temperature to_Kelvin();
	Temperature to_Fahrenheit();

	string in_what_range();

	Temperature get_temp_val(int = MIN)const;
	int get_temp_type() const;

	Temperature operator+(const Temperature&);
	Temperature operator-(const Temperature&);
	Temperature operator*(const Temperature&);
	Temperature operator/(const Temperature&);

	Temperature& operator+=(const Temperature&);
	Temperature& operator-=(const Temperature&);
	Temperature& operator*=(const Temperature&);
	Temperature& operator/=(const Temperature&);

	T& operator[](int);

	template <typename T>
	friend ostream& operator<<(ostream&, const Temperature<T>&);
	template <typename T>
	friend istream& operator>>(istream&, Temperature&);
};

/*
*@[brief]: Function converts current object to , and returns it  
*@[out]: converted to  object
*/
template <typename T>
Temperature<T> Temperature<T>::to_Celsius() {
	bool bad_type = false;
	if (is_celsius) {
		return *this;
	}
	Temperature<T> temp_obj(CELSIUS);
	switch (temp_type) {
	case KELVIN:
		temp_obj.temp = this->temp;
		temp_obj.temp -= 273.15; //Value needed for conversation
		break;
	case FAHRENHEIT:
		temp_obj.temp = this->temp;
		temp_obj.temp -= 32;	//Values needed
		temp_obj.temp *= 5;	   //for conversation
		temp_obj.temp /= 9;	  //formula
		break;
	default:
		bad_type = true;
		break;
	}
	if (bad_type) {
		cout << "Invalid type\n";
		return *this;
	}
	return temp_obj;
}

/*
*@[brief]: Function converts current object to fahrenheit, and returns it
*@[out]: converted to fahrenheit object
*/
template <typename T>
Temperature<T> Temperature<T>::to_Fahrenheit() {
	bool bad_type = false;
	if (is_fahrenheit) {
		return *this;
	}
	Temperature<T> temp_obj(FAHRENHEIT);
	switch (temp_type) {
	case KELVIN:
		temp_obj.temp = this->temp;
		temp_obj.temp *= 9;		  //Values needed
		temp_obj.temp /= 5;		 //for conversation
		temp_obj.temp -= 459.67;//formula
		break;
	case CELSIUS:
		temp_obj.temp = this->temp;
		temp_obj.temp *= 9;		//Values needed
		temp_obj.temp /= 5;	   //for conversation
		temp_obj.temp += 32;  //formula
		break;
	default:
		bad_type = true;
		break;
	}
	if (bad_type) {
		cout << "Invalid type\n";
		return *this;
	}
	return temp_obj;

}

/*
*@[brief]: Function converts current object to kelvin, and returns it
*@[out]: converted to kelvin object
*/
template <typename T>
Temperature<T> Temperature<T>::to_Kelvin() {

	bool bad_type = false;
	if (is_kelvin) {
		return *this;
	}
	Temperature<T> temp_obj(KELVIN);
	switch (temp_type) {
	case FAHRENHEIT:
		temp_obj.temp = this->temp;
		temp_obj.temp += 459.67; //Values needed
		temp_obj.temp *= 5;		//for convesation
		temp_obj.temp /= 9;	   //formula
		break;
	case CELSIUS:
		temp_obj.temp = this->temp;
		temp_obj.temp += 273.15;//Value needed for conversation
		break;
	default:
		bad_type = true;
		break;
	}
	if (bad_type) {
		cout << "Invalid type\n";
		return *this;
	}
	return temp_obj;
}



/*
*@[brief]: helper function append  range name depending on if it's first append or not
*@[in]: reference to appending string and range name
*/
template <typename T>
void Temperature<T>::append_range(string& range_var, string range) {
	if (range.empty()) {
		throw Temp_Exception{"Bad range name"};
	}
	if (range_var.empty()) {
		range_var.append(range);
	}
	else {
		range_var.append(" ");
		range_var.append(range);
	}
}

/*
*@[brief]: function check for all possible ranges and add them to string
*@[out]: string with ranges in what is out temperature
*/
template <typename T>
string Temperature<T>::in_what_range() {
	Temperature<T> tmp;
	string range;
	if (!is_celsius) {
		tmp = to_Celsius();
	}
	else {
		tmp = *this;
	}
	Ranges_Index id;
	int index{ 0 };
	try {
		if (is_range(tmp, COMMERCIAL)) {
			id = Ranges_Index::commercial;
			index = static_cast<int>(id);
			append_range(range, ranges_names.at(index));
		}
		if (is_range(tmp, MILITARY)) {
			id = Ranges_Index::military;
			index = static_cast<int>(id);
			append_range(range, ranges_names.at(index));
		}
		if (is_range(tmp, INDUSTRIAL)) {
			id = Ranges_Index::industrial;
			index = static_cast<int>(id);
			append_range(range, ranges_names.at(index));
		}
		if (is_range(tmp, EXTENDED)) {
			id = Ranges_Index::extended;
			index = static_cast<int>(id);
			append_range(range, ranges_names.at(index));
		}
		if (is_range(tmp, AUTOMOTIVE)) {
			id = Ranges_Index::automotive;
			index = static_cast<int>(id);
			append_range(range, ranges_names.at(index));
		}
		if (range.empty()) {
			id = Ranges_Index::not_in_range;
			index = static_cast<int>(id);
			append_range(range, ranges_names.at(index));
		}
		return range;
	}
	catch (exception& ex) {
		cerr << "We got exception here: " << ex.what() << ". Closing program\n";
		exit(true);
	}
}

/*
*@[brief]: function finds minimum or maximum depending on type parametr, from temperature list, default(MIN);
*@[out]: object that contains min or max temperature;
*/
template<typename T>
Temperature<T> Temperature<T>::get_temp_val(int type)const
{
	T val;
	switch (type) {
	case MIN:
		val = this->temp.min();
		break;
	case MAX:
		val = this->temp.max();
		break;
	default:
		cout << "invalid parametr in get_temp_val\n";
		exit(true);
	}
	Temperature<T> obj = *this;
	obj.temp.resize(1);
	obj.temp[0] = val;
	return obj;
}

/*
*@[brief]: helper function that returns current temper type
*@[out]: current value
*/
template<typename T>
int Temperature<T>::get_temp_type() const
{
	return temp_type;
}

/*
*@[brief]: overloaded operator + for temperature class;
*@[out]: exception will be thrown if temperature type will be different
*/
template<typename T>
Temperature<T> Temperature<T>::operator+(const Temperature & obj)
{
	if (this->temp_type != obj.temp_type) {
		throw Temp_Exception{"Operands temperature type mismatch"};
	}
	Temperature<T> tmp{ *this };
	tmp += obj;
	return tmp;
}

/*
*@[brief]: overloaded operator - for temperature class;
*@[out]: exception will be thrown if temperature type will be different
*/
template<typename T>
Temperature<T> Temperature<T>::operator-(const Temperature & obj)
{
	if (this->temp_type != obj.temp_type) {
		//	throw {"Operands temperature type mismatch"};
	}
	Temperature<T> tmp{ *this };
	tmp -= obj;
	return tmp;
}

/*
*@[brief]: overloaded operator * for temperature class;
*@[out]: exception will be thrown if temperature type will be different
*/
template<typename T>
Temperature<T> Temperature<T>::operator*(const Temperature & obj)
{
	if (this->temp_type != obj.temp_type) {
		throw Temp_Exception{ "Operands temperature type mismatch" };
	}
	Temperature<T> tmp{ *this };
	tmp *= obj;
	return tmp;
}

/*
*@[brief]: overloaded operator / for temperature class;
*@[out]: exception will be thrown if temperature type will be different
*/
template<typename T>
Temperature<T> Temperature<T>::operator/(const Temperature & obj)
{
	if (this->temp_type != obj.temp_type) {
		throw Temp_Exception{ "Operands temperature type mismatch" };
	}
	Temperature<T> tmp{ *this };
	tmp /= obj;
	return tmp;
}

/*
*@[brief]: overloaded operator += for temperature class;
*@[out]: exception will be thrown if temperature type will be different
*/
template<typename T>
Temperature<T>& Temperature<T>::operator+=(const Temperature & obj)
{
	if (this->temp_type != obj.temp_type) {
		throw Temp_Exception{ "Operands temperature type mismatch" };
	}
	this->temp += obj.temp;
	return *this;
}
/*
*@[brief]: overloaded operator -= for temperature class;
*@[out]: exception will be thrown if temperature type will be different
*/
template<typename T>
Temperature<T>& Temperature<T>::operator-=(const Temperature & obj)
{
	if (this->temp_type != obj.temp_type) {
		throw Temp_Exception{ "Operands temperature type mismatch" };
	}
	this->temp -= obj.temp;
	return *this;
}
/*
*@[brief]: overloaded operator *= for temperature class;
*@[out]: exception will be thrown if temperature type will be different
*/
template<typename T>
Temperature<T>& Temperature<T>::operator*=(const Temperature & obj)
{
	if (this->temp_type != obj.temp_type) {
		throw Temp_Exception{ "Operands temperature type mismatch" };
	}
	this->temp *= obj.temp;
	return *this;
}

/*
*@[brief]: overloaded operator /= for temperature class;
*@[out]: exception will be thrown if temperature type will be different
*/
template<typename T>
Temperature<T>& Temperature<T>::operator/=(const Temperature & obj)
{
	if (this->temp_type != obj.temp_type) {
		throw Temp_Exception{ "Operands temperature type mismatch" };
	}
	this->temp /= obj.temp;
	return *this;
}
/*
*@[brief]: overloaded subscription operator for temperature class;
*@[in]: index for subscription
*@[out]: exception will be thrown if index is negetive or bigger than size of temperature -1 else return reference to index value
*/
template<typename T>
T & Temperature<T>::operator[](int index)
{
	if (index<0 || index>temp.size()-1) {
		throw Temp_Exception{ "Temperature subscript out of range" };
	}
	return temp[index];
	// TODO: insert return statement here
}

/*
*@[brief]: function check if temperature is in given range, if no range given default(commercial) is used
*@[in]: Temperature object that holds temperature, int range type
*@[out]: true if in range else false
*/
template<typename T>
bool Temperature<T>::is_range(Temperature<T> obj, int range_type) {
	Temperature<T> temp_obj;
	if (!obj.temp.size()) {
		throw Temp_Exception{ "Empty temperature value" };
	}
	if (obj.temp.size() > 1) {
		temp_obj = obj.get_temp_val(MIN);
	}
	else {
		temp_obj = obj;
	}
	auto temper = temp_obj.temp[0];
	Ranges_Celsius low_range;
	Ranges_Celsius up_range;
	
	switch (range_type) {
	case COMMERCIAL:
		low_range = Ranges_Celsius::lower_commercial;
		up_range = Ranges_Celsius::upper_commercial;
		break;
	case MILITARY:
		low_range = Ranges_Celsius::lower_military;
		up_range = Ranges_Celsius::upper_military;
		break;
	case INDUSTRIAL:
		low_range = Ranges_Celsius::lower_intdustrial;
		up_range = Ranges_Celsius::upper_industrial;
		break;
	case EXTENDED:
		low_range = Ranges_Celsius::lower_extended;
		up_range = Ranges_Celsius::upper_exetended;
		break;
	case AUTOMOTIVE:
		low_range = Ranges_Celsius::lower_automotive;
		up_range = Ranges_Celsius::upper_automotive;
		break;
	default:
		cout << "Bad range type, setting to default(commercial)\n";
		low_range = Ranges_Celsius::lower_commercial;
		up_range = Ranges_Celsius::upper_commercial;
		break;
	}
	auto up_bound = static_cast<int>(up_range);
	auto low_bound = static_cast<int>(low_range);
	if (temper<low_bound || temper>up_bound)
		return false;
	return true;
}

/*
*@[brief]: Function check if istream object state is set to failbit, shows error and fix input stream;
*@[in]: istream object needed for fail check;
*@[out]: bool statement that shows function result, true if failbit enabled and false otherwise;
*/
bool isbadinput(istream& in) {
	if (in.fail()) {
		in.clear();
		in.ignore();
		return true;
	}
	return false;
}

/*
*@[brief]: overloaded input operator for class Temperature
*@[in]: istream object for input, const class object for buffer
*@[out]: istream object for chain input
*/
template <typename T>
istream& operator>>(istream& is, Temperature<T>& temper) {
	vector<T>tmp;
	T data;
	string dt;
	getline(cin, dt);
	istringstream ins(dt);
	while (ins >> data) {
		if (isbadinput(ins)) {
			continue;
		}
		tmp.emplace_back(data);
	}
	int i = 0;
	valarray<T> tmp2(tmp.size());
	for (const auto x : tmp) {
		tmp2[i++] = x;
	}
	temper = Temperature<T>{ tmp2,temper.get_temp_type()};
	return is;
}

/*
*@[brief]: overloaded output operator for class Temperature
*@[in]: ostream object for output, const class object for data
*@[out]: ostream object for chain output
*/
template <typename T>
ostream& operator<<(ostream& os, const Temperature<T>&temper) {
	for (const auto x : temper.temp) {
		os << x << temper.def_spec << " ";
	}
	return os;
}

/*
*@[brief]: Set flags that indicate current temperature type to false(zero)
*/
template<typename T>
void Temperature<T>::set_flags_zero() {
	is_celsius = false;
	is_fahrenheit = false;
	is_kelvin = false;
}


/*
*@[brief]: Constructor with parametrs with default celsius type
*@[in]: valarray<T> - array of temperatures, int - temperature type (-1 - kelvin, 0 - celsius(default), 1 - fahrenheit
*/
template<typename T>
Temperature<T>::Temperature(valarray<T> temp_arr, int type)
{
	set_flags_zero();
	switch (type) {
	case KELVIN:
		is_kelvin = true;
		def_spec = KELVIN_SPEC;
		break;
	case CELSIUS:
		is_celsius = true;
		def_spec = CELSIUS_SPEC;
		break;
	case FAHRENHEIT:
		is_fahrenheit = true;
		def_spec = FAHRENHEIT_SPEC;
		break;
	default:
		cout << "Bad temp type, setting default type (celsius)\n";
		type = CELSIUS;
		is_celsius = true;
		def_spec = CELSIUS_SPEC;
		break;
	}
	temp_type = type;
	temp = temp_arr;
}
/*
*@[brief]: Constructor with parametrs
*@[in]:  int - temperature type (-1 - kelvin, 0 - celsius, 1 - fahrenheit
*/
template<typename T>
Temperature<T>::Temperature(int type)
{
	set_flags_zero();
	switch (type) {
	case KELVIN:
		is_kelvin = true;
		def_spec = KELVIN_SPEC;
		break;
	case CELSIUS:
		is_celsius = true;
		def_spec = CELSIUS_SPEC;
		break;
	case FAHRENHEIT:
		is_fahrenheit = true;
		def_spec = FAHRENHEIT_SPEC;
		break;
	default:
		cout << "Bad temp type, setting default type (celsius)\n";
		type = CELSIUS;
		is_celsius = true;
		def_spec = CELSIUS_SPEC;
		break;
	}
	temp_type = type;
}

template<typename T>
Temperature<T>::~Temperature() {
	set_flags_zero();
	temp *= 0;
	temp.resize(0);
}

void main() {
	Temperature<double> obj({270, 600}, KELVIN);
	Temperature<double> obj1({ 270, 600 }, CELSIUS);
	try {
		cout << obj[6];
	}
	catch (exception& ex) {
		cout << ex.what();
	}
	//cin >> obj;
//	cout << obj.get_temp_val(MAX).in_what_range()<<"\n";
	}
