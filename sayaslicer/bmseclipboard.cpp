#include "bmseclipboard.hpp"

double BMSEClipboardObject::toSamplePosition(double bpm, int sampleRate, int numChannels) {
	return (60.0 / (double)bpm * ((double)sampleRate * (double)numChannels)) / 192.0 * 4.0 * (double)position;
}

std::string BMSEClipboardObject::toString() {
	char pos[8];
	snprintf(pos, 8, "%07d", position);
	return column + std::to_string(noteType) + std::string(pos) + std::to_string(value);
}

BMSEClipboardObject::BMSEClipboardObject(std::string column, int noteType, int position, int value) {
	this->column = column;
	this->noteType = noteType;
	this->position = position;
	this->value = value;
}

BMSEClipboardObject::BMSEClipboardObject(std::string s) {
	if (s.size() >= 12) {
		column = s.substr(0, 3);
		noteType = std::stoi(s.substr(3, 1));
		position = std::stoi(s.substr(4, 7));
		value = std::stoi(s.substr(11));
	}
}



iBMSCClipboardObject::iBMSCClipboardObject(int column, double position, int value) {
	this->column = column;
	this->position = position;
	this->value = value;
}

std::string iBMSCClipboardObject::toString() {
	char pos[64];
	snprintf(pos, 64, "%d %f %d0000 %d %d %d", column, position, value, ln_length, invisible ? -1 : 0, mine ? -1 : 0);
	return std::string(pos);
}



std::string BMSEClipboard::toBMSEClipboardData(MarkerList markers, double bpm, int sampleRate, int numChannels, int startDef) {
	std::string res = "BMSE ClipBoard Object Data Format\r\n";
	for (auto m : markers) {
		double samplesPer192th = 60.0 / (double)bpm * ((double)sampleRate * (double)numChannels) / 192.0 * 4.0;
		double position = m.position / samplesPer192th;
		BMSEClipboardObject o("101", 0, round(position), startDef++);
		res = res + o.toString() + "\r\n";
	}
	return res;
}

std::string BMSEClipboard::toiBMSCClipboardData(MarkerList markers, double bpm, int sampleRate, int numChannels, int startDef) {
	std::string res = "iBMSC Clipboard Data xNT\r\n";
	for (auto m : markers) {
		double samplesPer192th = 60.0 / (double)bpm * ((double)sampleRate * (double)numChannels) / 192.0 * 4.0;
		double position = m.position / samplesPer192th;
		iBMSCClipboardObject o(27, position, startDef++);
		res = res + o.toString() + "\r\n";
	}
	return res;
}

BMSEClipboard::BMSEClipboard(std::string s) {
	s.erase(std::remove(s.begin(), s.end(), '\r'), s.cend()); // Remove \r from the string
	std::stringstream ss(s);
	std::string l;
	auto lines = std::vector<std::string>{};
	while (std::getline(ss, l, '\n')) {
		lines.push_back(l);
	}
	if (lines.size() > 0 && lines[0] == "BMSE ClipBoard Object Data Format") {
		for (int i = 1; i < lines.size(); i++) {
			BMSEClipboardObject bo(lines[i]);
			if (!bo.column.empty())
				objects.push_back(bo);
		}
	}
	else {
		std::cout << "Invalid clipboard data" << std::endl;
	}
}



void AddMarkersFromBMSEClipboard(BMSEClipboard objs, SoundBuffer& buffer, SlicerSettings& settings) {
	if (buffer.getSampleCount() > 0) {
		auto sampleRate = buffer.getSampleRate();
		auto numChannels = buffer.getChannelCount();
		for (BMSEClipboardObject o : objs.objects) {
			double m = o.toSamplePosition(settings.bpm, sampleRate, numChannels);
			if (settings.markers.find(m) == -1.0) {
				settings.markers.push_back(m);
			}
		}
		InsertNotification({ ImGuiToastType::Success, 3000, "imported_markers_clipboard"_t.c_str() });
	}
	else {
		InsertNotification({ ImGuiToastType::Error, 3000, "load_file_first"_t.c_str() });
	}
}

void ProcessBMSEClipboard(SoundBuffer& buffer, SlicerSettings& settings) {
	std::string cb;
	clip::get_text(cb);
	BMSEClipboard objs(cb);
	if (!objs.objects.empty()) {
		AddMarkersFromBMSEClipboard(objs, buffer, settings);
		settings.updateHistory = true;
	}
	else
		InsertNotification({ ImGuiToastType::Error, 3000, "clipboard_no_bmse_data"_t.c_str() });
}

void GenerateBMSEClipboard(SoundBuffer& buffer, SlicerSettings settings, bool useiBMSC) {
	if (buffer.getSampleCount() > 0) {
		std::string cb;
		if (useiBMSC)
			cb = BMSEClipboard::toiBMSCClipboardData(settings.markers, settings.bpm, buffer.getSampleRate(), buffer.getChannelCount(), settings.startingKeysound);
		else
			cb = BMSEClipboard::toBMSEClipboardData(settings.markers, settings.bpm, buffer.getSampleRate(), buffer.getChannelCount(), settings.startingKeysound);
		std::cout << cb << std::endl;
		clip::set_text(cb);
		InsertNotification({ ImGuiToastType::Success, 3000, "%s", useiBMSC ? "copied_markers_ibmsc_data"_t.c_str() : "copied_markers_bmse_data"_t.c_str() });
	}
	else {
		InsertNotification({ ImGuiToastType::Error, 3000, "load_file_first"_t.c_str() });
	}
}