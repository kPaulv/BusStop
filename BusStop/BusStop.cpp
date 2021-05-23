#include <iostream>
#include <utility>
#include <cstring>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

//this class contains information about bus company, departure and arrival time
class Bus {
private:
	string companyName;
	pair<int, int> departure;
	pair<int, int> arrival;
public: 
	Bus() {
		companyName = "";
		departure.first = 0;
		departure.second = 0;
		arrival.first = 0;
		arrival.second = 0;
	}

	Bus(const Bus& object) {
		this->companyName = object.companyName;
		this->departure.first = object.departure.first;
		this->departure.second = object.departure.second;
		this->arrival.first = object.arrival.first;
		this->arrival.second = object.arrival.second;
	}

	string getCompanyName() {
		return this->companyName;
	}

	pair<int, int> getDeparture() {
		return this->departure;
	}

	pair<int, int> getArrival() {
		return this->arrival;
	}

	void setCompanyName(string name) {
		this->companyName = name;
	}

	void setDeparture(pair<int, int> departure) {
		this->departure.first = departure.first;
		this->departure.second = departure.second;
	}

	void setArrival(pair<int, int> arrival) {
		this->arrival.first = arrival.first;
		this->arrival.second = arrival.second;
	}
};

//exception if file isn't opened
struct FileException : public exception{
	string getMessage(){
		return "Error! Could not open file!";
	}
};

//shall we add the entry to the table
bool isEfficient(pair<int, int> departure, pair<int, int> arrival, Bus bus) {
	if ((departure == bus.getDeparture() && arrival == bus.getArrival()) ||
		(departure == bus.getDeparture() && arrival > bus.getArrival()) ||
		(departure < bus.getDeparture() && arrival == bus.getArrival())) {
		return false;
	}
	return true;
}

//shall we replace any entry with a more efficient one
bool isMoreEfficient(pair<int, int> departure, pair<int, int> arrival, Bus bus) {
	if ((departure == bus.getDeparture() && arrival < bus.getArrival()) || 
		(departure > bus.getDeparture() && arrival == bus.getArrival()) || 
		(departure > bus.getDeparture() && arrival < bus.getArrival())) {
		return true;
	}
	return false;
}

//sorting result timetable
void sortBuses(vector<Bus>& poshBuses, vector<Bus>& grottyBuses) {
	if (!poshBuses.empty()) {
		Bus temp;
		int j;
		for (unsigned int i = 1; i < poshBuses.size(); i++) {
			j = i;
			while (j > 0 && poshBuses[j - 1].getDeparture() > poshBuses[j].getDeparture()) {
				temp = poshBuses[j];
				poshBuses[j] = poshBuses[j - 1];
				poshBuses[j - 1] = temp;
			}
		}
	}
	if (!grottyBuses.empty()) {
		Bus temp;
		int j;
		for (unsigned int i = 1; i < grottyBuses.size(); i++) {
			j = i;
			while (j > 0 && grottyBuses[j - 1].getDeparture() > grottyBuses[j].getDeparture()) {
				temp = grottyBuses[j];
				grottyBuses[j] = grottyBuses[j - 1];
				grottyBuses[j - 1] = temp;
			}
		}
	}
}

int main() {
	try {
		ifstream fin("input.txt");
		if (!fin.is_open()) {
			throw FileException();
		}
		vector<Bus> poshBuses;
		vector<Bus> grottyBuses;
		Bus tempBus;
		string name;
		string departureLine;
		string arrivalLine;
		pair<int, int> departure;
		pair<int, int> arrival;
		string token;
		int pos = 0;
		int count = 0;
		bool efficient;
		int index = 0;
		bool added;
		while (!fin.eof()) {
			efficient = true;
			added = false;
			fin >> name >> departureLine >> arrivalLine;

			//split departure/arrival time into hours and minutes
			pos = departureLine.find(":");
			departure.first = stoi(departureLine.substr(0, pos));
			arrival.first = stoi(arrivalLine.substr(0, pos));
			departureLine.erase(0, pos + 1);
			arrivalLine.erase(0, pos + 1);
			departure.second = stoi(departureLine.substr(0, departureLine.length()));
			arrival.second = stoi(arrivalLine.substr(0, arrivalLine.length()));

			//services longer than an hour should not be included
			if (arrival.first - departure.first > 1) {
				continue;
			} else if (arrival.first - departure.first == 1 && arrival.second > departure.second) {
				continue;
			} else if (arrival.first < departure.first || (arrival.first == departure.first && arrival.second < departure.second)) {
				continue;
			}

			//Posh buses should be used over Grotty buses
			if (name == "Grotty" && !poshBuses.empty()) { //if the entry is already in poshBuses
				for (vector<Bus>::iterator it = poshBuses.begin(); it != poshBuses.end(); it++) {
					if (departure == (*it).getDeparture() && arrival == (*it).getArrival()) {
						efficient = false;
						break;
					}
				}
				if (!efficient) {
					continue;
				}
			} else if (name == "Posh" && !grottyBuses.empty()) { //if the entry is already in grottyBuses
				for (vector<Bus>::iterator it = grottyBuses.begin(); it != grottyBuses.end(); it++) {
					if (departure == (*it).getDeparture() && arrival == (*it).getArrival()) {
						grottyBuses.erase(it);
						break;
					}
				}
			}

			//checking is an entry efficient and is it more efficient than exsisting ones in posh buses
			if (name == "Posh" && !poshBuses.empty()) {
				for (vector<Bus>::iterator it = poshBuses.begin(); it != poshBuses.end(); it++) {
					if (!isEfficient(departure, arrival, *it)) {
						efficient = false;
						break;
					}
					if (isMoreEfficient(departure, arrival, *it)) {
						index = distance(poshBuses.begin(), it);
						tempBus.setCompanyName(name);
						tempBus.setDeparture(departure);
						tempBus.setArrival(arrival);
						poshBuses.at(index) = tempBus;
						added = true;
						break;
					}
				}
				if (!efficient) {
					continue;
				}
				if (added) {
					continue;
				}
			}

			//checking is an entry efficient and is it more efficient than exsisting ones in grotty buses
			if (name == "Grotty" && !grottyBuses.empty()) {
				for (vector<Bus>::iterator it = grottyBuses.begin(); it != grottyBuses.end(); it++) {
					if (!isEfficient(departure, arrival, *it)) {
						efficient = false;
						break;
					}
					if (isMoreEfficient(departure, arrival, *it)) {
						index = distance(grottyBuses.begin(), it);
						tempBus.setCompanyName(name);
						tempBus.setDeparture(departure);
						tempBus.setArrival(arrival);
						grottyBuses.at(index) = tempBus;
						added = true;
						break;
					}
				}
				if (!efficient) {
					continue;
				}
				if (added) {
					continue;
				}
			}

			tempBus.setCompanyName(name);
			tempBus.setDeparture(departure);
			tempBus.setArrival(arrival);

			if (name == "Posh") {
				poshBuses.push_back(tempBus);
			}
			else {
				grottyBuses.push_back(tempBus);
			}
		}
		fin.close();

		//sorting buses in a departure time order
		sortBuses(poshBuses, grottyBuses);

		ofstream fout("output.txt");
		if (!fout.is_open()) {
			throw FileException();
		}
		//formatted output
		if (!poshBuses.empty()) {
			for (vector<Bus>::iterator it = poshBuses.begin(); it != poshBuses.end(); it++) {
				fout << (*it).getCompanyName() << " ";
				//departure
				if ((*it).getDeparture().first < 10) {
					fout << "0" << (*it).getDeparture().first << ":";
				}
				else {
					fout << (*it).getDeparture().first << ":";
				}
				if ((*it).getDeparture().second < 10) {
					fout << "0" << (*it).getDeparture().second << " ";
				}
				else {
					fout << (*it).getDeparture().second << " ";
				}
				//arrival
				if ((*it).getArrival().first < 10) {
					fout << "0" << (*it).getArrival().first << ":";
				}
				else {
					fout << (*it).getArrival().first << ":";
				}
				if ((*it).getArrival().second < 10) {
					fout << "0" << (*it).getArrival().second;
				}
				else {
					fout << (*it).getArrival().second;
				}
				
				if (it != poshBuses.end() - 1 || (it == poshBuses.end() - 1 && !grottyBuses.empty())) {
					fout << endl;
				}
			}
		}
		if (!grottyBuses.empty()) {
			if (!poshBuses.empty()) {
				fout << endl;
			}
			for (vector<Bus>::iterator it = grottyBuses.begin(); it != grottyBuses.end(); it++) {
				fout << (*it).getCompanyName() << " ";

				//departure
				if ((*it).getDeparture().first < 10) {
					fout << "0" << (*it).getDeparture().first << ":";
				}
				else {
					fout << (*it).getDeparture().first << ":";
				}
				if ((*it).getDeparture().second < 10) {
					fout << "0" << (*it).getDeparture().second << " ";
				}
				else {
					fout << (*it).getDeparture().second << " ";
				}
				//arrival
				if ((*it).getArrival().first < 10) {
					fout << "0" << (*it).getArrival().first << ":";
				}
				else {
					fout << (*it).getArrival().first << ":";
				}
				if ((*it).getArrival().second < 10) {
					fout << "0" << (*it).getArrival().second;
				}
				else {
					fout << (*it).getArrival().second;
				}

				if (it != grottyBuses.end() - 1) {
					fout << endl;
				}
			}
		}
		fout.close();
	}
	catch (FileException& e){
		cout << e.getMessage();
	}
}