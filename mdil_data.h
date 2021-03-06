#pragma once

#pragma pack(push,1)
struct	mdil_header
{
	DWORD	hdrSize;	// size of header in bytes
	DWORD	magic;	// ��MDIL��
	DWORD	version;	// currently 0x00020006
	DWORD	typeMapCount;	// number of entries in type map section
	DWORD	methodMapCount;	// number of entries in method map section
	DWORD	genericInstSize;	// size of generic inst section
	DWORD	extModRefsCount;	// number of entries in external module sect.
	DWORD	extTypeRefsCount;	// number of entries in external type section
	DWORD	extMemberRefsCount;	// number of entries in external member sect.
	DWORD	typeSpecCount;	// number of entries in typespec section
	DWORD	methodSpecCount ;	// number of entries in methodspec section
	DWORD	signatureCount;	
	DWORD	namePoolSize;	// size of name pool in bytes
	DWORD	typeSize;	// size of compact type layout section
	DWORD	userStringPoolSize;	// size of user string pool
	DWORD	codeSize;		// size of MDIL code
	DWORD	stubSize;
	DWORD	stubAssocSize;
	DWORD	debugMapCount;	// number of entries in debug info map
	DWORD	debugInfoSize;	// size of debug info
	DWORD	timeDateStamp;
	DWORD	subsystem;	
	LPCVOID	baseAddress;	
	DWORD	entryPointToken;	// method def token for the entry point or 0

	enum mdil_header_flags
	{
		EntryPointReturnsVoid	= 0x01,
		WellKnownTypesPresent	= 0x02,
		TargetArch_Mask	= 0x0c,
		TargetArch_X86	= 0x00,
		TargetArch_AMD64	= 0x04,
		TargetArch_IA64	= 0x08,
		// room for future TargetArch_...   // it looks like TargetArch_ARM = _AMD64 | _IA64 !? 
		DebuggableILAssembly	= 0x10,	// Assembly created with /debug
		DebuggableMDILCode	= 0x20,	// MDIL file created with /debug
		IsHDRfile	= 0x40,
	};

	DWORD	flags;
	DWORD	cerReliabilityContract;

	enum PlatformID
	{
		PlatformID_Unknown	= 0,
		PlatformID_Triton	= 1,
		PlatformID_Redhawk	= 2,
	};

	DWORD	platformID;
	DWORD	platformDataSize;
	DWORD	genericCodeSize;
	DWORD	genericDebugInfoSize;
	WORD	compilerMajorVersion;
	WORD	compilerMinorVersion;
	WORD	compilerBuild;
	WORD	compilerRevision;
	DWORD	subVersion;
};

struct mdil_header_2
{
	DWORD	size; // should be 120 bytes
	DWORD	field_04;
	DWORD	field_08;
	DWORD	field_0c;
	DWORD	field_10;
	DWORD	field_14;
	DWORD	field_18;
	DWORD	field_1c;
	DWORD	field_20;
	DWORD	field_24;
	DWORD	field_28;
	DWORD	field_2c;
	DWORD	field_30;
	DWORD	field_34;
	DWORD	field_38;
	DWORD	field_3c;
	DWORD	field_40;
	DWORD	field_44;
	DWORD	field_48;
	DWORD	field_4c;
	DWORD	field_50;
	DWORD	field_54;
	DWORD	field_58;
	DWORD	field_5c;
	DWORD	field_60;
	DWORD	section_21_count;
	DWORD	section_22_count;
	DWORD	field_6c;
	DWORD	field_70;
	DWORD	field_74;
};
#pragma pack(pop)

struct	ExtModRef
{
	ULONG	ModName;
	ULONG	RefName;
};

struct	ExtTypeRef
{
	ULONG	module	: 14;	// 16383 max modules to import from
	ULONG	ordinal	: 18;	// 262143 max types within a module
};

struct ExtMemberRef
{
	ULONG	extTypeRid : 15;	// 32767 max types to import
	ULONG	isTypeSpec : 1;	// refers to typespec?
	ULONG	isField : 1;	// is this a field or a	method?
	ULONG	ordinal : 15;	// 32767 max fields or	methods in a type
};

template<typename T>
class shared_vector : public std::shared_ptr<std::vector<T>> {
public:
	typename std::vector<T>::size_type size() const { return get() ? get()->size() : 0; }
	void resize(typename std::vector<T>::size_type size) { if (get()) get()->resize(size); else reset(new std::vector<T>(size)); }
};

//////////////////////////////////////////////////////////////////////////
// Type Spec

struct mdil_type_spec
{
	const mdTypeSpec token;
	const CorElementType element_type;
	mdil_type_spec(const mdTypeSpec _token, CorElementType _type) : token(_token), element_type(_type) {}
};

struct mdil_type_spec_with_type : mdil_type_spec
{
	const mdToken type_token;
	mdil_type_spec_with_type(const mdTypeSpec _token, const CorElementType _type, const mdToken _type_token) : mdil_type_spec(_token, _type), type_token(_type_token) {}
};

struct mdil_type_spec_with_number : mdil_type_spec
{
	const uint32_t number;
	mdil_type_spec_with_number(const mdTypeSpec _token, const CorElementType _type, const uint32_t _number) : mdil_type_spec(_token, _type), number(_number) {}
};

struct mdil_type_spec_with_child : mdil_type_spec
{
	const std::shared_ptr<mdil_type_spec> child;
	mdil_type_spec_with_child(const mdTypeSpec _token, const CorElementType _type, const std::shared_ptr<mdil_type_spec>& _child) : mdil_type_spec(_token, _type), child(_child) {}
};

struct mdil_type_spec_array : mdil_type_spec_with_child
{
	const uint32_t rank;
	const std::vector<uint32_t> bounds;
	const std::vector<uint32_t> lbounds;
	mdil_type_spec_array(const mdTypeSpec _token, const std::shared_ptr<mdil_type_spec>& _child, const uint32_t _rank)
		: mdil_type_spec_with_child(_token, ELEMENT_TYPE_ARRAY, _child), rank(_rank) {}
	mdil_type_spec_array(const mdTypeSpec _token, const std::shared_ptr<mdil_type_spec>& _child, const uint32_t _rank, const std::vector<uint32_t>& _bounds, const std::vector<uint32_t>& _lbounds)
		: mdil_type_spec_with_child(_token, ELEMENT_TYPE_ARRAY, _child), rank(_rank), bounds(_bounds), lbounds(_lbounds) {}
};

struct mdil_type_spec_generic : mdil_type_spec_with_child
{
	const std::vector<std::shared_ptr<mdil_type_spec>> type_arguments;
	mdil_type_spec_generic(const mdTypeSpec _token, const std::shared_ptr<mdil_type_spec>& _child)
		: mdil_type_spec_with_child(_token, ELEMENT_TYPE_GENERICINST, _child) {}
	mdil_type_spec_generic(const mdTypeSpec _token, const std::shared_ptr<mdil_type_spec>& _child, const std::vector<std::shared_ptr<mdil_type_spec>>& _args)
		: mdil_type_spec_with_child(_token, ELEMENT_TYPE_GENERICINST, _child), type_arguments(_args) {}
};

struct mdil_type_specs
{
	shared_vector<unsigned long> raw;
	shared_vector<std::shared_ptr<mdil_type_spec>> type_specs;
};

//////////////////////////////////////////////////////////////////////////
// Method Spec

struct mdil_method_spec
{
	mdToken method_token;
	shared_vector<std::shared_ptr<mdil_type_spec>> parameters;
};

struct mdil_method_specs
{
	shared_vector<unsigned long> raw;
	shared_vector<std::shared_ptr<mdil_method_spec>> method_specs;
};

//////////////////////////////////////////////////////////////////////////
// Type Def

struct mdil_field_def
{
	mdFieldDef token;
	std::unique_ptr<uint32_t> explicit_offset;
	enum field_storage
	{
		fsInstance,
		fsStatic,
		fsThreadLocal,
		fsContextLocal,
		fsRVA
	} storage;
	enum field_protection
	{
		fpPrivateScope,
		fpPrivate,
		fpFam_AND_Assem,
		fpAssembly,
		fpFamily,
		fpFam_OR_Assem,
		fpPublic
	} protection;
	CorElementType element_type;
	std::shared_ptr<mdToken> boxing_type_token;
};

struct mdil_generic_param
{
	const mdGenericParam token;
	const CorGenericParamAttr attributes; 
	mdil_generic_param(const uint32_t _token, const CorGenericParamAttr _attributes) : token(_token), attributes(_attributes) {}
};

struct mdil_method_def
{
	enum method_kind
	{
		mkNormal,
		mkPInvoke,
		mkNativeCallable,
		mkRuntimeImport,
		mkRuntimeExport,
		mkImplement,
		mkImplementInterface,
	} kind;
	mdMethodDef token;
	CorMethodAttr attributes;
	CorMethodImpl impl_attributes;
	mdToken overridden_method_token;
	enum method_impl_hints
	{
		mihIL,
		mihFCall,
		mihNDirect,
		mihEEImpl,
		mihArray,
		mihInstantiated,
		mihComInterop,
		mihDynamic,
		mihCtor = 0x10,
		mihDefault_Ctor = 0x20,
		mihCCtor = 0x40,
		mihRetObj = 0x100,
		mihRetByRef = 0x200,
		mihBy_Ordinal = 0x1000,
	} impl_hints;
	uint32_t module_name;
	uint32_t entry_point_name;
	CorUnmanagedCallingConvention calling_convention;
	shared_vector<std::shared_ptr<const mdil_generic_param>> generic_params;

	mdTypeDef type_token;
};

struct mdil_type_guid {
	uint8_t guid[16];
	uint32_t unknown;
};

struct mdil_type_def
{
	mdTypeDef token;
	mdTypeDef enclosing_type_token;
	std::shared_ptr<uint32_t> layout_pack;
	std::shared_ptr<uint32_t> layout_size;
	std::shared_ptr<uint32_t> extended_flags;
	std::shared_ptr<uint32_t> winrt_redirected;
	CorTypeAttr attributes;
	mdToken base_type_token;
	std::shared_ptr<mdil_type_guid> guid_information;
	shared_vector<std::shared_ptr<const mdil_generic_param>> generic_params;
	std::vector<std::shared_ptr<const mdil_field_def>> fields;
	std::vector<std::shared_ptr<const mdil_method_def>> methods;
	std::vector<std::shared_ptr<const mdToken>> impl_interfaces;
	std::vector<std::shared_ptr<const mdil_method_def>> impl_interface_methods;
	mdil_type_def(mdTypeDef _token) : token(_token), attributes(tdNotPublic), base_type_token(mdTokenNil) {}
};

struct mdil_type_defs
{
	shared_vector<unsigned long> raw;
	shared_vector<const std::shared_ptr<mdil_type_def>> type_defs;
};

//////////////////////////////////////////////////////////////////////////
// Method Map

struct mdil_method_def_mapping
{
	std::shared_ptr<mdil_method_def> method_def;
	bool is_generic_inst : 1;
	uint32_t offset : 31;
};

struct mdil_method_map
{
	shared_vector<unsigned long> raw;
	shared_vector<const std::shared_ptr<mdil_method_def_mapping>> method_def_mappings;
};

//////////////////////////////////////////////////////////////////////////
// Generic Instances

struct mdil_generic_instance
{
	uint32_t code_offset;
	uint32_t debug_offset;
	std::vector<uint32_t> argument_types;
};

struct mdil_generic_method
{
	uint32_t offset;
	uint16_t instance_count;
	uint8_t flags;
	uint8_t argument_count;
	std::vector<std::shared_ptr<mdil_generic_instance>> instances;
};

struct mdil_generic_instances
{
	shared_vector<unsigned char> raw;
	std::unordered_map<uint32_t, std::shared_ptr<mdil_generic_method>> generic_methods;
};

//////////////////////////////////////////////////////////////////////////
// Code

enum mdil_architecture { archX86 = 1, archX64 = 2, archARM = 3 };

struct mdil_instruction {
	enum instr_language {
		ilMDIL, ilX86, ilX64, ilARM
	} const language;

	const uint32_t offset;
	uint32_t length; // if =0 means it has error, decoding failed
	std::string opcode;
	std::string operands;
	
	enum instr_ref_type {
		rtNone, rtMetadataToken, rtJumpDistance,
	} ref_type;
	uint32_t ref_value;

	void set(const std::string&& opcode) {
		this->opcode = move(opcode);
	}

	void set(const std::string&& opcode, const std::string&& operands) {
		this->opcode = move(opcode);
		this->operands = move(operands);
	}

	void setref(instr_ref_type type, uint32_t value) { ref_type = type; ref_value = value; }

	mdil_instruction(instr_language _language, uint32_t _offset) : language(_language), offset(_offset), length(0), ref_type(rtNone), ref_value(0) {}
};

struct mdil_method
{
	size_t offset;
	size_t global_offset;
	size_t size;

	size_t routine_offset;
	size_t routine_size;
	unsigned long exception_count;

	std::vector<std::shared_ptr<mdil_instruction>> routine;

	mdil_method(size_t _offset, size_t _global_offset, size_t _size, size_t _routine_offset, size_t _routine_size, unsigned long _exception_count)
		: offset(_offset), global_offset(_global_offset), size(_size), routine_offset(_routine_offset), routine_size(_routine_size), exception_count(_exception_count) {}
};

struct mdil_code
{
	shared_vector<unsigned char> raw;
	std::vector<mdil_method> methods;
};

//////////////////////////////////////////////////////////////////////////
// Assembly

class mdil_data {
public:
	std::shared_ptr<mdil_header>	header;
	std::shared_ptr<mdil_header_2>	header_2;
	shared_vector<unsigned char>	platform_data;
	shared_vector<unsigned long>	well_known_types;
	mdil_type_defs					type_map;
	mdil_method_map					method_map;
	mdil_generic_instances				generic_instances;
	shared_vector<ExtModRef>		ext_module_refs;
	shared_vector<ExtTypeRef>		ext_type_refs;
	shared_vector<ExtMemberRef>		ext_member_refs;
	mdil_type_specs					type_specs;
	mdil_method_specs				method_specs;
	shared_vector<unsigned long>	signatures;
	shared_vector<char>				name_pool;
	shared_vector<unsigned char>	types;
	shared_vector<char>				user_string_pool;
	mdil_code						code_1;
	mdil_code						code_2;
	shared_vector<unsigned char>	stubs;
	shared_vector<unsigned char>	stubAssocs;
	shared_vector<unsigned long>	debug_map;
	shared_vector<unsigned char>	debug_info_1;
	shared_vector<unsigned char>	debug_info_2;
	shared_vector<unsigned char>	section_21;
	shared_vector<unsigned char>	section_22;
};


