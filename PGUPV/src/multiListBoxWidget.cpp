#include "multiListBoxWidget.h"

#include <guipg.h>

bool PGUPV::_multilistboxBegin(uint32_t id, const std::string& label) {
	return PGUPV::GUILib::MultiListBoxBegin(id, label);
}


bool PGUPV::_multilistboxItem(const std::string& element, bool& selected) {
	return PGUPV::GUILib::MultiListBoxItem(element, &selected);
}


void _multilistboxItem() {
	return PGUPV::GUILib::MultiListBoxEnd();
}
