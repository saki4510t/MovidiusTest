/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_DESCRIPTOR_H_
#define UAC_DESCRIPTOR_H_

#include "utilbase.h"
// core
#include "core/descriptor.h"
// uac
#include "aanduac.h"

namespace serenegiant {
namespace usb {
namespace uac {

class AudioControlInterface;
class AudioStreamInterface;
class UACStream;

/**
*
*/
class UACDescriptor : public Descriptor {
friend UACStream;
private:
	bool _is_uac;
	bool parse_ac;
	AudioControlInterface *ac_interface;
	bool parse_as;
	AudioStreamInterface *as_interface;	// これは作業用なので解析中以外は使わないこと
	//
	void dump_ac_header(const uac1_ac_header_descriptor_t *desc);
	void dump_ac_input_terminal(const uac1_input_terminal_descriptor_t *desc);
	void dump_ac_output_terminal(const uac1_output_terminal_descriptor_t *desc);
	void dump_ac_mixer_unit(const uac1_mixer_unit_descriptor_t *desc);
	void dump_ac_selector_unit(const uac1_selector_unit_descriptor_t *desc);
	void dump_ac_feature_unit(const uac1_feature_unit_descriptor_t *desc);
	void dump_ac_processing_unit(const uac1_processing_unit_descriptor_t *desc);
	void dump_ac_extension_unit(const uac1_extension_unit_descriptor_t *desc);
	//
	void dump_as_descriptor_header(const uac_descriptor_header_t *_desc);
	void dump_as_header(const uac1_as_header_descriptor_t *desc);
	void dump_as_format(const uac_format_descriptor *desc);
	void dump_as_foramt_type_i_continuous(const uac_format_i_continuous_descriptor_t *desc);
	void dump_as_foramt_type_i_discrete(const uac_format_i_discrete_descriptor_t *desc);
	void dump_as_foramt_type_i_ext(const uac_format_i_ext_descriptor_t *desc);
	void dump_as_foramt_type_ii_continuous(const uac_format_ii_continuous_descriptor_t *desc);
	void dump_as_foramt_type_ii_discrete(const uac_format_ii_discrete_descriptor_t *desc);
	void dump_as_foramt_type_ii_ext(const uac_format_ii_ext_descriptor_t *desc);
	//
	void dump_iso_endpoint(const uac_iso_endpoint_descriptor_t *desc);
	void dump_status_word(const uac1_status_word_t *desc);
protected:
	// general
//	virtual void parse_device_descriptor(descriptor_iter &_iter, const device_descriptor_t *_desc);
//	virtual void parse_config_descriptor(descriptor_iter &_iter, const config_descriptor_t *_desc);
//	virtual void parse_string_descriptor(descriptor_iter &_iter, const string_descriptor_t *_desc);
//	virtual void parse_interface_association_descriptor(descriptor_iter &_iter, const interface_assoc_descriptor_t *_desc);
	virtual Interface *parse_interface_descriptor(descriptor_iter &_iter, const interface_descriptor_t *_desc);
	virtual Endpoint *parse_endpoint_descriptor(descriptor_iter &_iter,
		const interface_descriptor_t *intf_desc,
		const endpoint_descriptor_t *_desc);
	// class specific
//	virtual void parse_cs_device_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
//	virtual void parse_cs_config_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
//	virtual void parse_cs_string_descriptor(descriptor_iter &_iter, const string_descriptor_t *_desc);
	virtual void parse_cs_interface_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
	virtual void parse_cs_endpoint_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
//
	Interface *parse_ac_interface(descriptor_iter& _iter, const interface_descriptor_t* _desc);
	Interface *parse_as_interface(descriptor_iter &_iter, const interface_descriptor_t *_desc);
public:
	UACDescriptor();
	virtual ~UACDescriptor();
	inline const bool is_uac() const { return _is_uac; };
	AudioControlInterface *get_ctrl_interface();
	const std::vector<AudioStreamInterface *> get_stream_interface();
	const std::vector<AudioStreamInterface *> get_midi_interface();
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_DESCRIPTOR_H_ */
