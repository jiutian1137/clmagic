#pragma once
#ifndef clmagic_directX12_ENUM_STRING_h_
#define clmagic_directX12_ENUM_STRING_h_
#include <d3d12.h>
#include <string>

namespace dx12 {
    struct enum_string : public std::string {
        using _Mybase = std::string;

        enum_string(D3D12_COMMAND_LIST_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_COMMAND_LIST_TYPE_DIRECT:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_TYPE_DIRECT";
				break;
			case D3D12_COMMAND_LIST_TYPE_BUNDLE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_TYPE_BUNDLE";
				break;
			case D3D12_COMMAND_LIST_TYPE_COMPUTE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_TYPE_COMPUTE";
				break;
			case D3D12_COMMAND_LIST_TYPE_COPY:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_TYPE_COPY";
				break;
			case D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE";
				break;
			case D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS";
				break;
			case D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_COMMAND_LIST_TYPE)]");
			};
		}

		enum_string(D3D12_COMMAND_QUEUE_FLAGS _Flags) {
			if (_Flags & D3D12_COMMAND_QUEUE_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_COMMAND_QUEUE_FLAG_NONE|";
			if (_Flags & D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT)
				static_cast<_Mybase&>(*this) += "D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_COMMAND_QUEUE_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_COMMAND_QUEUE_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_COMMAND_QUEUE_PRIORITY _Flags) {
			switch (_Flags) {
			case D3D12_COMMAND_QUEUE_PRIORITY_NORMAL:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_QUEUE_PRIORITY_NORMAL";
				break;
			case D3D12_COMMAND_QUEUE_PRIORITY_HIGH:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_QUEUE_PRIORITY_HIGH";
				break;
			case D3D12_COMMAND_QUEUE_PRIORITY_GLOBAL_REALTIME:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_QUEUE_PRIORITY_GLOBAL_REALTIME";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_COMMAND_QUEUE_PRIORITY)]");
			};
		}

		enum_string(D3D12_PRIMITIVE_TOPOLOGY_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED:
				static_cast<_Mybase&>(*this) = "D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED";
				break;
			case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:
				static_cast<_Mybase&>(*this) = "D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT";
				break;
			case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:
				static_cast<_Mybase&>(*this) = "D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE";
				break;
			case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:
				static_cast<_Mybase&>(*this) = "D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE";
				break;
			case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH:
				static_cast<_Mybase&>(*this) = "D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_PRIMITIVE_TOPOLOGY_TYPE)]");
			};
		}

		enum_string(D3D12_INPUT_CLASSIFICATION _Flags) {
			switch (_Flags) {
			case D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA:
				static_cast<_Mybase&>(*this) = "D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA";
				break;
			case D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA:
				static_cast<_Mybase&>(*this) = "D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_INPUT_CLASSIFICATION)]");
			};
		}

		enum_string(D3D12_FILL_MODE _Flags) {
			switch (_Flags) {
			case D3D12_FILL_MODE_WIREFRAME:
				static_cast<_Mybase&>(*this) = "D3D12_FILL_MODE_WIREFRAME";
				break;
			case D3D12_FILL_MODE_SOLID:
				static_cast<_Mybase&>(*this) = "D3D12_FILL_MODE_SOLID";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_FILL_MODE)]");
			};
		}

		enum_string(D3D12_CULL_MODE _Flags) {
			switch (_Flags) {
			case D3D12_CULL_MODE_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_CULL_MODE_NONE";
				break;
			case D3D12_CULL_MODE_FRONT:
				static_cast<_Mybase&>(*this) = "D3D12_CULL_MODE_FRONT";
				break;
			case D3D12_CULL_MODE_BACK:
				static_cast<_Mybase&>(*this) = "D3D12_CULL_MODE_BACK";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_CULL_MODE)]");
			};
		}

		enum_string(D3D12_COMPARISON_FUNC _Flags) {
			switch (_Flags) {
			case D3D12_COMPARISON_FUNC_NEVER:
				static_cast<_Mybase&>(*this) = "D3D12_COMPARISON_FUNC_NEVER";
				break;
			case D3D12_COMPARISON_FUNC_LESS:
				static_cast<_Mybase&>(*this) = "D3D12_COMPARISON_FUNC_LESS";
				break;
			case D3D12_COMPARISON_FUNC_EQUAL:
				static_cast<_Mybase&>(*this) = "D3D12_COMPARISON_FUNC_EQUAL";
				break;
			case D3D12_COMPARISON_FUNC_LESS_EQUAL:
				static_cast<_Mybase&>(*this) = "D3D12_COMPARISON_FUNC_LESS_EQUAL";
				break;
			case D3D12_COMPARISON_FUNC_GREATER:
				static_cast<_Mybase&>(*this) = "D3D12_COMPARISON_FUNC_GREATER";
				break;
			case D3D12_COMPARISON_FUNC_NOT_EQUAL:
				static_cast<_Mybase&>(*this) = "D3D12_COMPARISON_FUNC_NOT_EQUAL";
				break;
			case D3D12_COMPARISON_FUNC_GREATER_EQUAL:
				static_cast<_Mybase&>(*this) = "D3D12_COMPARISON_FUNC_GREATER_EQUAL";
				break;
			case D3D12_COMPARISON_FUNC_ALWAYS:
				static_cast<_Mybase&>(*this) = "D3D12_COMPARISON_FUNC_ALWAYS";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_COMPARISON_FUNC)]");
			};
		}

		enum_string(D3D12_DEPTH_WRITE_MASK _Flags) {
			switch (_Flags) {
			case D3D12_DEPTH_WRITE_MASK_ZERO:
				static_cast<_Mybase&>(*this) = "D3D12_DEPTH_WRITE_MASK_ZERO";
				break;
			case D3D12_DEPTH_WRITE_MASK_ALL:
				static_cast<_Mybase&>(*this) = "D3D12_DEPTH_WRITE_MASK_ALL";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DEPTH_WRITE_MASK)]");
			};
		}

		enum_string(D3D12_STENCIL_OP _Flags) {
			switch (_Flags) {
			case D3D12_STENCIL_OP_KEEP:
				static_cast<_Mybase&>(*this) = "D3D12_STENCIL_OP_KEEP";
				break;
			case D3D12_STENCIL_OP_ZERO:
				static_cast<_Mybase&>(*this) = "D3D12_STENCIL_OP_ZERO";
				break;
			case D3D12_STENCIL_OP_REPLACE:
				static_cast<_Mybase&>(*this) = "D3D12_STENCIL_OP_REPLACE";
				break;
			case D3D12_STENCIL_OP_INCR_SAT:
				static_cast<_Mybase&>(*this) = "D3D12_STENCIL_OP_INCR_SAT";
				break;
			case D3D12_STENCIL_OP_DECR_SAT:
				static_cast<_Mybase&>(*this) = "D3D12_STENCIL_OP_DECR_SAT";
				break;
			case D3D12_STENCIL_OP_INVERT:
				static_cast<_Mybase&>(*this) = "D3D12_STENCIL_OP_INVERT";
				break;
			case D3D12_STENCIL_OP_INCR:
				static_cast<_Mybase&>(*this) = "D3D12_STENCIL_OP_INCR";
				break;
			case D3D12_STENCIL_OP_DECR:
				static_cast<_Mybase&>(*this) = "D3D12_STENCIL_OP_DECR";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_STENCIL_OP)]");
			};
		}

		enum_string(D3D12_BLEND _Flags) {
			switch (_Flags) {
			case D3D12_BLEND_ZERO:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_ZERO";
				break;
			case D3D12_BLEND_ONE:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_ONE";
				break;
			case D3D12_BLEND_SRC_COLOR:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_SRC_COLOR";
				break;
			case D3D12_BLEND_INV_SRC_COLOR:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_INV_SRC_COLOR";
				break;
			case D3D12_BLEND_SRC_ALPHA:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_SRC_ALPHA";
				break;
			case D3D12_BLEND_INV_SRC_ALPHA:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_INV_SRC_ALPHA";
				break;
			case D3D12_BLEND_DEST_ALPHA:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_DEST_ALPHA";
				break;
			case D3D12_BLEND_INV_DEST_ALPHA:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_INV_DEST_ALPHA";
				break;
			case D3D12_BLEND_DEST_COLOR:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_DEST_COLOR";
				break;
			case D3D12_BLEND_INV_DEST_COLOR:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_INV_DEST_COLOR";
				break;
			case D3D12_BLEND_SRC_ALPHA_SAT:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_SRC_ALPHA_SAT";
				break;
			case D3D12_BLEND_BLEND_FACTOR:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_BLEND_FACTOR";
				break;
			case D3D12_BLEND_INV_BLEND_FACTOR:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_INV_BLEND_FACTOR";
				break;
			case D3D12_BLEND_SRC1_COLOR:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_SRC1_COLOR";
				break;
			case D3D12_BLEND_INV_SRC1_COLOR:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_INV_SRC1_COLOR";
				break;
			case D3D12_BLEND_SRC1_ALPHA:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_SRC1_ALPHA";
				break;
			case D3D12_BLEND_INV_SRC1_ALPHA:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_INV_SRC1_ALPHA";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_BLEND)]");
			};
		}

		enum_string(D3D12_BLEND_OP _Flags) {
			switch (_Flags) {
			case D3D12_BLEND_OP_ADD:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_OP_ADD";
				break;
			case D3D12_BLEND_OP_SUBTRACT:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_OP_SUBTRACT";
				break;
			case D3D12_BLEND_OP_REV_SUBTRACT:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_OP_REV_SUBTRACT";
				break;
			case D3D12_BLEND_OP_MIN:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_OP_MIN";
				break;
			case D3D12_BLEND_OP_MAX:
				static_cast<_Mybase&>(*this) = "D3D12_BLEND_OP_MAX";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_BLEND_OP)]");
			};
		}

		enum_string(D3D12_COLOR_WRITE_ENABLE _Flags) {
			switch (_Flags) {
			case D3D12_COLOR_WRITE_ENABLE_RED:
				static_cast<_Mybase&>(*this) = "D3D12_COLOR_WRITE_ENABLE_RED";
				break;
			case D3D12_COLOR_WRITE_ENABLE_GREEN:
				static_cast<_Mybase&>(*this) = "D3D12_COLOR_WRITE_ENABLE_GREEN";
				break;
			case D3D12_COLOR_WRITE_ENABLE_BLUE:
				static_cast<_Mybase&>(*this) = "D3D12_COLOR_WRITE_ENABLE_BLUE";
				break;
			case D3D12_COLOR_WRITE_ENABLE_ALPHA:
				static_cast<_Mybase&>(*this) = "D3D12_COLOR_WRITE_ENABLE_ALPHA";
				break;
			case D3D12_COLOR_WRITE_ENABLE_ALL:
				static_cast<_Mybase&>(*this) = "D3D12_COLOR_WRITE_ENABLE_ALL";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_COLOR_WRITE_ENABLE)]");
			};
		}

		enum_string(D3D12_LOGIC_OP _Flags) {
			switch (_Flags) {
			case D3D12_LOGIC_OP_CLEAR:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_CLEAR";
				break;
			case D3D12_LOGIC_OP_SET:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_SET";
				break;
			case D3D12_LOGIC_OP_COPY:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_COPY";
				break;
			case D3D12_LOGIC_OP_COPY_INVERTED:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_COPY_INVERTED";
				break;
			case D3D12_LOGIC_OP_NOOP:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_NOOP";
				break;
			case D3D12_LOGIC_OP_INVERT:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_INVERT";
				break;
			case D3D12_LOGIC_OP_AND:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_AND";
				break;
			case D3D12_LOGIC_OP_NAND:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_NAND";
				break;
			case D3D12_LOGIC_OP_OR:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_OR";
				break;
			case D3D12_LOGIC_OP_NOR:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_NOR";
				break;
			case D3D12_LOGIC_OP_XOR:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_XOR";
				break;
			case D3D12_LOGIC_OP_EQUIV:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_EQUIV";
				break;
			case D3D12_LOGIC_OP_AND_REVERSE:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_AND_REVERSE";
				break;
			case D3D12_LOGIC_OP_AND_INVERTED:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_AND_INVERTED";
				break;
			case D3D12_LOGIC_OP_OR_REVERSE:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_OR_REVERSE";
				break;
			case D3D12_LOGIC_OP_OR_INVERTED:
				static_cast<_Mybase&>(*this) = "D3D12_LOGIC_OP_OR_INVERTED";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_LOGIC_OP)]");
			};
		}

		enum_string(D3D12_CONSERVATIVE_RASTERIZATION_MODE _Flags) {
			switch (_Flags) {
			case D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF:
				static_cast<_Mybase&>(*this) = "D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF";
				break;
			case D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON:
				static_cast<_Mybase&>(*this) = "D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_CONSERVATIVE_RASTERIZATION_MODE)]");
			};
		}

		enum_string(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE _Flags) {
			switch (_Flags) {
			case D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED:
				static_cast<_Mybase&>(*this) = "D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED";
				break;
			case D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF:
				static_cast<_Mybase&>(*this) = "D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF";
				break;
			case D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF:
				static_cast<_Mybase&>(*this) = "D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE)]");
			};
		}

		enum_string(D3D12_PIPELINE_STATE_FLAGS _Flags) {
			if (_Flags & D3D12_PIPELINE_STATE_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_PIPELINE_STATE_FLAG_NONE|";
			if (_Flags & D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG)
				static_cast<_Mybase&>(*this) += "D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_PIPELINE_STATE_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING";
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MAX_VALID:
				static_cast<_Mybase&>(*this) = "D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MAX_VALID";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE)]");
			};
		}

		enum_string(D3D12_FEATURE _Flags) {
			switch (_Flags) {
			case D3D12_FEATURE_D3D12_OPTIONS:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_D3D12_OPTIONS";
				break;
			case D3D12_FEATURE_ARCHITECTURE:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_ARCHITECTURE";
				break;
			case D3D12_FEATURE_FEATURE_LEVELS:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_FEATURE_LEVELS";
				break;
			case D3D12_FEATURE_FORMAT_SUPPORT:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_FORMAT_SUPPORT";
				break;
			case D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS";
				break;
			case D3D12_FEATURE_FORMAT_INFO:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_FORMAT_INFO";
				break;
			case D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT";
				break;
			case D3D12_FEATURE_SHADER_MODEL:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_SHADER_MODEL";
				break;
			case D3D12_FEATURE_D3D12_OPTIONS1:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_D3D12_OPTIONS1";
				break;
			case D3D12_FEATURE_PROTECTED_RESOURCE_SESSION_SUPPORT:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_PROTECTED_RESOURCE_SESSION_SUPPORT";
				break;
			case D3D12_FEATURE_ROOT_SIGNATURE:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_ROOT_SIGNATURE";
				break;
			case D3D12_FEATURE_ARCHITECTURE1:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_ARCHITECTURE1";
				break;
			case D3D12_FEATURE_D3D12_OPTIONS2:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_D3D12_OPTIONS2";
				break;
			case D3D12_FEATURE_SHADER_CACHE:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_SHADER_CACHE";
				break;
			case D3D12_FEATURE_COMMAND_QUEUE_PRIORITY:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_COMMAND_QUEUE_PRIORITY";
				break;
			case D3D12_FEATURE_D3D12_OPTIONS3:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_D3D12_OPTIONS3";
				break;
			case D3D12_FEATURE_EXISTING_HEAPS:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_EXISTING_HEAPS";
				break;
			case D3D12_FEATURE_D3D12_OPTIONS4:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_D3D12_OPTIONS4";
				break;
			case D3D12_FEATURE_SERIALIZATION:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_SERIALIZATION";
				break;
			case D3D12_FEATURE_CROSS_NODE:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_CROSS_NODE";
				break;
			case D3D12_FEATURE_D3D12_OPTIONS5:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_D3D12_OPTIONS5";
				break;
			case D3D12_FEATURE_D3D12_OPTIONS6:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_D3D12_OPTIONS6";
				break;
			case D3D12_FEATURE_QUERY_META_COMMAND:
				static_cast<_Mybase&>(*this) = "D3D12_FEATURE_QUERY_META_COMMAND";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_FEATURE)]");
			};
		}

		enum_string(D3D12_SHADER_MIN_PRECISION_SUPPORT _Flags) {
			if (_Flags & D3D12_SHADER_MIN_PRECISION_SUPPORT_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_SHADER_MIN_PRECISION_SUPPORT_NONE|";
			if (_Flags & D3D12_SHADER_MIN_PRECISION_SUPPORT_10_BIT)
				static_cast<_Mybase&>(*this) += "D3D12_SHADER_MIN_PRECISION_SUPPORT_10_BIT|";
			if (_Flags & D3D12_SHADER_MIN_PRECISION_SUPPORT_16_BIT)
				static_cast<_Mybase&>(*this) += "D3D12_SHADER_MIN_PRECISION_SUPPORT_16_BIT|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_SHADER_MIN_PRECISION_SUPPORT_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_SHADER_MIN_PRECISION_SUPPORT)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_TILED_RESOURCES_TIER _Flags) {
			switch (_Flags) {
			case D3D12_TILED_RESOURCES_TIER_NOT_SUPPORTED:
				static_cast<_Mybase&>(*this) = "D3D12_TILED_RESOURCES_TIER_NOT_SUPPORTED";
				break;
			case D3D12_TILED_RESOURCES_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_TILED_RESOURCES_TIER_1";
				break;
			case D3D12_TILED_RESOURCES_TIER_2:
				static_cast<_Mybase&>(*this) = "D3D12_TILED_RESOURCES_TIER_2";
				break;
			case D3D12_TILED_RESOURCES_TIER_3:
				static_cast<_Mybase&>(*this) = "D3D12_TILED_RESOURCES_TIER_3";
				break;
			case D3D12_TILED_RESOURCES_TIER_4:
				static_cast<_Mybase&>(*this) = "D3D12_TILED_RESOURCES_TIER_4";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_TILED_RESOURCES_TIER)]");
			};
		}

		enum_string(D3D12_RESOURCE_BINDING_TIER _Flags) {
			switch (_Flags) {
			case D3D12_RESOURCE_BINDING_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_BINDING_TIER_1";
				break;
			case D3D12_RESOURCE_BINDING_TIER_2:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_BINDING_TIER_2";
				break;
			case D3D12_RESOURCE_BINDING_TIER_3:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_BINDING_TIER_3";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESOURCE_BINDING_TIER)]");
			};
		}

		enum_string(D3D12_CONSERVATIVE_RASTERIZATION_TIER _Flags) {
			switch (_Flags) {
			case D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED:
				static_cast<_Mybase&>(*this) = "D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED";
				break;
			case D3D12_CONSERVATIVE_RASTERIZATION_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_CONSERVATIVE_RASTERIZATION_TIER_1";
				break;
			case D3D12_CONSERVATIVE_RASTERIZATION_TIER_2:
				static_cast<_Mybase&>(*this) = "D3D12_CONSERVATIVE_RASTERIZATION_TIER_2";
				break;
			case D3D12_CONSERVATIVE_RASTERIZATION_TIER_3:
				static_cast<_Mybase&>(*this) = "D3D12_CONSERVATIVE_RASTERIZATION_TIER_3";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_CONSERVATIVE_RASTERIZATION_TIER)]");
			};
		}

		enum_string(D3D12_FORMAT_SUPPORT1 _Flags) {
			if (_Flags & D3D12_FORMAT_SUPPORT1_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_NONE|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_BUFFER)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_BUFFER|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_IA_VERTEX_BUFFER)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_IA_VERTEX_BUFFER|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_IA_INDEX_BUFFER)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_IA_INDEX_BUFFER|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_SO_BUFFER)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_SO_BUFFER|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_TEXTURE1D)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_TEXTURE1D|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_TEXTURE2D)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_TEXTURE2D|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_TEXTURE3D)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_TEXTURE3D|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_TEXTURECUBE)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_TEXTURECUBE|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_SHADER_LOAD)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_SHADER_LOAD|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE_COMPARISON)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE_COMPARISON|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE_MONO_TEXT)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE_MONO_TEXT|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_MIP)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_MIP|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_RENDER_TARGET)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_RENDER_TARGET|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_BLENDABLE)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_BLENDABLE|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_MULTISAMPLE_RESOLVE)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_MULTISAMPLE_RESOLVE|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_DISPLAY)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_DISPLAY|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_CAST_WITHIN_BIT_LAYOUT)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_CAST_WITHIN_BIT_LAYOUT|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_MULTISAMPLE_RENDERTARGET)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_MULTISAMPLE_RENDERTARGET|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_MULTISAMPLE_LOAD)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_MULTISAMPLE_LOAD|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_SHADER_GATHER)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_SHADER_GATHER|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_BACK_BUFFER_CAST)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_BACK_BUFFER_CAST|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_SHADER_GATHER_COMPARISON)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_SHADER_GATHER_COMPARISON|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_DECODER_OUTPUT)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_DECODER_OUTPUT|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_VIDEO_PROCESSOR_OUTPUT)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_VIDEO_PROCESSOR_OUTPUT|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_VIDEO_PROCESSOR_INPUT)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_VIDEO_PROCESSOR_INPUT|";
			if (_Flags & D3D12_FORMAT_SUPPORT1_VIDEO_ENCODER)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT1_VIDEO_ENCODER|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_FORMAT_SUPPORT1_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_FORMAT_SUPPORT1)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_FORMAT_SUPPORT2 _Flags) {
			if (_Flags & D3D12_FORMAT_SUPPORT2_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_NONE|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_ADD)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_ADD|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_BITWISE_OPS)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_BITWISE_OPS|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_COMPARE_STORE_OR_COMPARE_EXCHANGE)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_COMPARE_STORE_OR_COMPARE_EXCHANGE|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_EXCHANGE)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_EXCHANGE|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_SIGNED_MIN_OR_MAX)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_SIGNED_MIN_OR_MAX|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_UNSIGNED_MIN_OR_MAX)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_UNSIGNED_MIN_OR_MAX|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_OUTPUT_MERGER_LOGIC_OP)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_OUTPUT_MERGER_LOGIC_OP|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_TILED)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_TILED|";
			if (_Flags & D3D12_FORMAT_SUPPORT2_MULTIPLANE_OVERLAY)
				static_cast<_Mybase&>(*this) += "D3D12_FORMAT_SUPPORT2_MULTIPLANE_OVERLAY|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_FORMAT_SUPPORT2_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_FORMAT_SUPPORT2)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS _Flags) {
			if (_Flags & D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE|";
			if (_Flags & D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_TILED_RESOURCE)
				static_cast<_Mybase&>(*this) += "D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_TILED_RESOURCE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_CROSS_NODE_SHARING_TIER _Flags) {
			switch (_Flags) {
			case D3D12_CROSS_NODE_SHARING_TIER_NOT_SUPPORTED:
				static_cast<_Mybase&>(*this) = "D3D12_CROSS_NODE_SHARING_TIER_NOT_SUPPORTED";
				break;
			case D3D12_CROSS_NODE_SHARING_TIER_1_EMULATED:
				static_cast<_Mybase&>(*this) = "D3D12_CROSS_NODE_SHARING_TIER_1_EMULATED";
				break;
			case D3D12_CROSS_NODE_SHARING_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_CROSS_NODE_SHARING_TIER_1";
				break;
			case D3D12_CROSS_NODE_SHARING_TIER_2:
				static_cast<_Mybase&>(*this) = "D3D12_CROSS_NODE_SHARING_TIER_2";
				break;
			case D3D12_CROSS_NODE_SHARING_TIER_3:
				static_cast<_Mybase&>(*this) = "D3D12_CROSS_NODE_SHARING_TIER_3";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_CROSS_NODE_SHARING_TIER)]");
			};
		}

		enum_string(D3D12_RESOURCE_HEAP_TIER _Flags) {
			switch (_Flags) {
			case D3D12_RESOURCE_HEAP_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_HEAP_TIER_1";
				break;
			case D3D12_RESOURCE_HEAP_TIER_2:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_HEAP_TIER_2";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESOURCE_HEAP_TIER)]");
			};
		}

		enum_string(D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER _Flags) {
			switch (_Flags) {
			case D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_NOT_SUPPORTED:
				static_cast<_Mybase&>(*this) = "D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_NOT_SUPPORTED";
				break;
			case D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_1";
				break;
			case D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_2:
				static_cast<_Mybase&>(*this) = "D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_2";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER)]");
			};
		}

		enum_string(D3D12_VIEW_INSTANCING_TIER _Flags) {
			switch (_Flags) {
			case D3D12_VIEW_INSTANCING_TIER_NOT_SUPPORTED:
				static_cast<_Mybase&>(*this) = "D3D12_VIEW_INSTANCING_TIER_NOT_SUPPORTED";
				break;
			case D3D12_VIEW_INSTANCING_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_VIEW_INSTANCING_TIER_1";
				break;
			case D3D12_VIEW_INSTANCING_TIER_2:
				static_cast<_Mybase&>(*this) = "D3D12_VIEW_INSTANCING_TIER_2";
				break;
			case D3D12_VIEW_INSTANCING_TIER_3:
				static_cast<_Mybase&>(*this) = "D3D12_VIEW_INSTANCING_TIER_3";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_VIEW_INSTANCING_TIER)]");
			};
		}

		enum_string(D3D_ROOT_SIGNATURE_VERSION _Flags) {
			if (_Flags & D3D_ROOT_SIGNATURE_VERSION_1)
				static_cast<_Mybase&>(*this) += "D3D_ROOT_SIGNATURE_VERSION_1|";
			if (_Flags & D3D_ROOT_SIGNATURE_VERSION_1_0)
				static_cast<_Mybase&>(*this) += "D3D_ROOT_SIGNATURE_VERSION_1_0|";
			if (_Flags & D3D_ROOT_SIGNATURE_VERSION_1_1)
				static_cast<_Mybase&>(*this) += "D3D_ROOT_SIGNATURE_VERSION_1_1|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D_ROOT_SIGNATURE_VERSION)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D_SHADER_MODEL _Flags) {
			if (_Flags & D3D_SHADER_MODEL_5_1)
				static_cast<_Mybase&>(*this) += "D3D_SHADER_MODEL_5_1|";
			if (_Flags & D3D_SHADER_MODEL_6_0)
				static_cast<_Mybase&>(*this) += "D3D_SHADER_MODEL_6_0|";
			if (_Flags & D3D_SHADER_MODEL_6_1)
				static_cast<_Mybase&>(*this) += "D3D_SHADER_MODEL_6_1|";
			if (_Flags & D3D_SHADER_MODEL_6_2)
				static_cast<_Mybase&>(*this) += "D3D_SHADER_MODEL_6_2|";
			if (_Flags & D3D_SHADER_MODEL_6_3)
				static_cast<_Mybase&>(*this) += "D3D_SHADER_MODEL_6_3|";
			if (_Flags & D3D_SHADER_MODEL_6_4)
				static_cast<_Mybase&>(*this) += "D3D_SHADER_MODEL_6_4|";
			if (_Flags & D3D_SHADER_MODEL_6_5)
				static_cast<_Mybase&>(*this) += "D3D_SHADER_MODEL_6_5|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D_SHADER_MODEL)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_SHADER_CACHE_SUPPORT_FLAGS _Flags) {
			if (_Flags & D3D12_SHADER_CACHE_SUPPORT_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_SHADER_CACHE_SUPPORT_NONE|";
			if (_Flags & D3D12_SHADER_CACHE_SUPPORT_SINGLE_PSO)
				static_cast<_Mybase&>(*this) += "D3D12_SHADER_CACHE_SUPPORT_SINGLE_PSO|";
			if (_Flags & D3D12_SHADER_CACHE_SUPPORT_LIBRARY)
				static_cast<_Mybase&>(*this) += "D3D12_SHADER_CACHE_SUPPORT_LIBRARY|";
			if (_Flags & D3D12_SHADER_CACHE_SUPPORT_AUTOMATIC_INPROC_CACHE)
				static_cast<_Mybase&>(*this) += "D3D12_SHADER_CACHE_SUPPORT_AUTOMATIC_INPROC_CACHE|";
			if (_Flags & D3D12_SHADER_CACHE_SUPPORT_AUTOMATIC_DISK_CACHE)
				static_cast<_Mybase&>(*this) += "D3D12_SHADER_CACHE_SUPPORT_AUTOMATIC_DISK_CACHE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_SHADER_CACHE_SUPPORT_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_SHADER_CACHE_SUPPORT_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_COMMAND_LIST_SUPPORT_FLAGS _Flags) {
			switch (_Flags) {
			case D3D12_COMMAND_LIST_SUPPORT_FLAG_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_SUPPORT_FLAG_NONE";
				break;
			case D3D12_COMMAND_LIST_SUPPORT_FLAG_DIRECT:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_SUPPORT_FLAG_DIRECT";
				break;
			case D3D12_COMMAND_LIST_SUPPORT_FLAG_BUNDLE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_SUPPORT_FLAG_BUNDLE";
				break;
			case D3D12_COMMAND_LIST_SUPPORT_FLAG_COMPUTE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_SUPPORT_FLAG_COMPUTE";
				break;
			case D3D12_COMMAND_LIST_SUPPORT_FLAG_COPY:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_SUPPORT_FLAG_COPY";
				break;
			case D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_DECODE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_DECODE";
				break;
			case D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_PROCESS:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_PROCESS";
				break;
			case D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_ENCODE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_ENCODE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_COMMAND_LIST_SUPPORT_FLAGS)]");
			};
		}

		enum_string(D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER _Flags) {
			switch (_Flags) {
			case D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_0:
				static_cast<_Mybase&>(*this) = "D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_0";
				break;
			case D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_1";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER)]");
			};
		}

		enum_string(D3D12_HEAP_SERIALIZATION_TIER _Flags) {
			switch (_Flags) {
			case D3D12_HEAP_SERIALIZATION_TIER_0:
				static_cast<_Mybase&>(*this) = "D3D12_HEAP_SERIALIZATION_TIER_0";
				break;
			case D3D12_HEAP_SERIALIZATION_TIER_10:
				static_cast<_Mybase&>(*this) = "D3D12_HEAP_SERIALIZATION_TIER_10";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_HEAP_SERIALIZATION_TIER)]");
			};
		}

		enum_string(D3D12_RENDER_PASS_TIER _Flags) {
			switch (_Flags) {
			case D3D12_RENDER_PASS_TIER_0:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_TIER_0";
				break;
			case D3D12_RENDER_PASS_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_TIER_1";
				break;
			case D3D12_RENDER_PASS_TIER_2:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_TIER_2";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RENDER_PASS_TIER)]");
			};
		}

		enum_string(D3D12_RAYTRACING_TIER _Flags) {
			switch (_Flags) {
			case D3D12_RAYTRACING_TIER_NOT_SUPPORTED:
				static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_TIER_NOT_SUPPORTED";
				break;
			case D3D12_RAYTRACING_TIER_1_0:
				static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_TIER_1_0";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RAYTRACING_TIER)]");
			};
		}

		enum_string(D3D12_VARIABLE_SHADING_RATE_TIER _Flags) {
			switch (_Flags) {
			case D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED:
				static_cast<_Mybase&>(*this) = "D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED";
				break;
			case D3D12_VARIABLE_SHADING_RATE_TIER_1:
				static_cast<_Mybase&>(*this) = "D3D12_VARIABLE_SHADING_RATE_TIER_1";
				break;
			case D3D12_VARIABLE_SHADING_RATE_TIER_2:
				static_cast<_Mybase&>(*this) = "D3D12_VARIABLE_SHADING_RATE_TIER_2";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_VARIABLE_SHADING_RATE_TIER)]");
			};
		}

		enum_string(D3D12_HEAP_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_HEAP_TYPE_DEFAULT:
				static_cast<_Mybase&>(*this) = "D3D12_HEAP_TYPE_DEFAULT";
				break;
			case D3D12_HEAP_TYPE_UPLOAD:
				static_cast<_Mybase&>(*this) = "D3D12_HEAP_TYPE_UPLOAD";
				break;
			case D3D12_HEAP_TYPE_READBACK:
				static_cast<_Mybase&>(*this) = "D3D12_HEAP_TYPE_READBACK";
				break;
			case D3D12_HEAP_TYPE_CUSTOM:
				static_cast<_Mybase&>(*this) = "D3D12_HEAP_TYPE_CUSTOM";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_HEAP_TYPE)]");
			};
		}

		enum_string(D3D12_CPU_PAGE_PROPERTY _Flags) {
			switch (_Flags) {
			case D3D12_CPU_PAGE_PROPERTY_UNKNOWN:
				static_cast<_Mybase&>(*this) = "D3D12_CPU_PAGE_PROPERTY_UNKNOWN";
				break;
			case D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE:
				static_cast<_Mybase&>(*this) = "D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE";
				break;
			case D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE:
				static_cast<_Mybase&>(*this) = "D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE";
				break;
			case D3D12_CPU_PAGE_PROPERTY_WRITE_BACK:
				static_cast<_Mybase&>(*this) = "D3D12_CPU_PAGE_PROPERTY_WRITE_BACK";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_CPU_PAGE_PROPERTY)]");
			};
		}

		enum_string(D3D12_MEMORY_POOL _Flags) {
			switch (_Flags) {
			case D3D12_MEMORY_POOL_UNKNOWN:
				static_cast<_Mybase&>(*this) = "D3D12_MEMORY_POOL_UNKNOWN";
				break;
			case D3D12_MEMORY_POOL_L0:
				static_cast<_Mybase&>(*this) = "D3D12_MEMORY_POOL_L0";
				break;
			case D3D12_MEMORY_POOL_L1:
				static_cast<_Mybase&>(*this) = "D3D12_MEMORY_POOL_L1";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_MEMORY_POOL)]");
			};
		}

		enum_string(D3D12_HEAP_FLAGS _Flags) {
			if (_Flags & D3D12_HEAP_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_NONE|";
			if (_Flags & D3D12_HEAP_FLAG_SHARED)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_SHARED|";
			if (_Flags & D3D12_HEAP_FLAG_DENY_BUFFERS)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_DENY_BUFFERS|";
			if (_Flags & D3D12_HEAP_FLAG_ALLOW_DISPLAY)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_ALLOW_DISPLAY|";
			if (_Flags & D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER|";
			if (_Flags & D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES|";
			if (_Flags & D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES|";
			if (_Flags & D3D12_HEAP_FLAG_HARDWARE_PROTECTED)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_HARDWARE_PROTECTED|";
			if (_Flags & D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH|";
			if (_Flags & D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS|";
			if (_Flags & D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES|";
			if (_Flags & D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS|";
			if (_Flags & D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES|";
			if (_Flags & D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES)
				static_cast<_Mybase&>(*this) += "D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_HEAP_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RESOURCE_DIMENSION _Flags) {
			switch (_Flags) {
			case D3D12_RESOURCE_DIMENSION_UNKNOWN:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_DIMENSION_UNKNOWN";
				break;
			case D3D12_RESOURCE_DIMENSION_BUFFER:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_DIMENSION_BUFFER";
				break;
			case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_DIMENSION_TEXTURE1D";
				break;
			case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_DIMENSION_TEXTURE2D";
				break;
			case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_DIMENSION_TEXTURE3D";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESOURCE_DIMENSION)]");
			};
		}

		enum_string(D3D12_TEXTURE_LAYOUT _Flags) {
			switch (_Flags) {
			case D3D12_TEXTURE_LAYOUT_UNKNOWN:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_LAYOUT_UNKNOWN";
				break;
			case D3D12_TEXTURE_LAYOUT_ROW_MAJOR:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_LAYOUT_ROW_MAJOR";
				break;
			case D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE";
				break;
			case D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_TEXTURE_LAYOUT)]");
			};
		}

		enum_string(D3D12_RESOURCE_FLAGS _Flags) {
			if (_Flags & D3D12_RESOURCE_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_FLAG_NONE|";
			if (_Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET|";
			if (_Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL|";
			if (_Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS|";
			if (_Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE|";
			if (_Flags & D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER|";
			if (_Flags & D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS|";
			if (_Flags & D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESOURCE_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_TILE_RANGE_FLAGS _Flags) {
			switch (_Flags) {
			case D3D12_TILE_RANGE_FLAG_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_TILE_RANGE_FLAG_NONE";
				break;
			case D3D12_TILE_RANGE_FLAG_NULL:
				static_cast<_Mybase&>(*this) = "D3D12_TILE_RANGE_FLAG_NULL";
				break;
			case D3D12_TILE_RANGE_FLAG_SKIP:
				static_cast<_Mybase&>(*this) = "D3D12_TILE_RANGE_FLAG_SKIP";
				break;
			case D3D12_TILE_RANGE_FLAG_REUSE_SINGLE_TILE:
				static_cast<_Mybase&>(*this) = "D3D12_TILE_RANGE_FLAG_REUSE_SINGLE_TILE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_TILE_RANGE_FLAGS)]");
			};
		}

		enum_string(D3D12_TILE_MAPPING_FLAGS _Flags) {
			if (_Flags & D3D12_TILE_MAPPING_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_TILE_MAPPING_FLAG_NONE|";
			if (_Flags & D3D12_TILE_MAPPING_FLAG_NO_HAZARD)
				static_cast<_Mybase&>(*this) += "D3D12_TILE_MAPPING_FLAG_NO_HAZARD|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_TILE_MAPPING_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_TILE_MAPPING_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_TILE_COPY_FLAGS _Flags) {
			if (_Flags & D3D12_TILE_COPY_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_TILE_COPY_FLAG_NONE|";
			if (_Flags & D3D12_TILE_COPY_FLAG_NO_HAZARD)
				static_cast<_Mybase&>(*this) += "D3D12_TILE_COPY_FLAG_NO_HAZARD|";
			if (_Flags & D3D12_TILE_COPY_FLAG_LINEAR_BUFFER_TO_SWIZZLED_TILED_RESOURCE)
				static_cast<_Mybase&>(*this) += "D3D12_TILE_COPY_FLAG_LINEAR_BUFFER_TO_SWIZZLED_TILED_RESOURCE|";
			if (_Flags & D3D12_TILE_COPY_FLAG_SWIZZLED_TILED_RESOURCE_TO_LINEAR_BUFFER)
				static_cast<_Mybase&>(*this) += "D3D12_TILE_COPY_FLAG_SWIZZLED_TILED_RESOURCE_TO_LINEAR_BUFFER|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_TILE_COPY_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_TILE_COPY_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RESOURCE_STATES _Flags) {
			if (_Flags & D3D12_RESOURCE_STATE_COMMON)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_COMMON|";
			if (_Flags & D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER|";
			if (_Flags & D3D12_RESOURCE_STATE_INDEX_BUFFER)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_INDEX_BUFFER|";
			if (_Flags & D3D12_RESOURCE_STATE_RENDER_TARGET)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_RENDER_TARGET|";
			if (_Flags & D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_UNORDERED_ACCESS|";
			if (_Flags & D3D12_RESOURCE_STATE_DEPTH_WRITE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_DEPTH_WRITE|";
			if (_Flags & D3D12_RESOURCE_STATE_DEPTH_READ)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_DEPTH_READ|";
			if (_Flags & D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE|";
			if (_Flags & D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE|";
			if (_Flags & D3D12_RESOURCE_STATE_STREAM_OUT)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_STREAM_OUT|";
			if (_Flags & D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT|";
			if (_Flags & D3D12_RESOURCE_STATE_COPY_DEST)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_COPY_DEST|";
			if (_Flags & D3D12_RESOURCE_STATE_COPY_SOURCE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_COPY_SOURCE|";
			if (_Flags & D3D12_RESOURCE_STATE_RESOLVE_DEST)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_RESOLVE_DEST|";
			if (_Flags & D3D12_RESOURCE_STATE_RESOLVE_SOURCE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_RESOLVE_SOURCE|";
			if (_Flags & D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE|";
			if (_Flags & D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE|";
			if (_Flags & D3D12_RESOURCE_STATE_GENERIC_READ)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_GENERIC_READ|";
			if (_Flags & D3D12_RESOURCE_STATE_PRESENT)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_PRESENT|";
			if (_Flags & D3D12_RESOURCE_STATE_PREDICATION)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_PREDICATION|";
			if (_Flags & D3D12_RESOURCE_STATE_VIDEO_DECODE_READ)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_VIDEO_DECODE_READ|";
			if (_Flags & D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE|";
			if (_Flags & D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ|";
			if (_Flags & D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE|";
			if (_Flags & D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ|";
			if (_Flags & D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_STATE_PRESENT";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESOURCE_STATES)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RESOURCE_BARRIER_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_RESOURCE_BARRIER_TYPE_TRANSITION:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_BARRIER_TYPE_TRANSITION";
				break;
			case D3D12_RESOURCE_BARRIER_TYPE_ALIASING:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_BARRIER_TYPE_ALIASING";
				break;
			case D3D12_RESOURCE_BARRIER_TYPE_UAV:
				static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_BARRIER_TYPE_UAV";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESOURCE_BARRIER_TYPE)]");
			};
		}

		enum_string(D3D12_RESOURCE_BARRIER_FLAGS _Flags) {
			if (_Flags & D3D12_RESOURCE_BARRIER_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_BARRIER_FLAG_NONE|";
			if (_Flags & D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY|";
			if (_Flags & D3D12_RESOURCE_BARRIER_FLAG_END_ONLY)
				static_cast<_Mybase&>(*this) += "D3D12_RESOURCE_BARRIER_FLAG_END_ONLY|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RESOURCE_BARRIER_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESOURCE_BARRIER_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_TEXTURE_COPY_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX";
				break;
			case D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_TEXTURE_COPY_TYPE)]");
			};
		}

		enum_string(D3D12_RESOLVE_MODE _Flags) {
			switch (_Flags) {
			case D3D12_RESOLVE_MODE_DECOMPRESS:
				static_cast<_Mybase&>(*this) = "D3D12_RESOLVE_MODE_DECOMPRESS";
				break;
			case D3D12_RESOLVE_MODE_MIN:
				static_cast<_Mybase&>(*this) = "D3D12_RESOLVE_MODE_MIN";
				break;
			case D3D12_RESOLVE_MODE_MAX:
				static_cast<_Mybase&>(*this) = "D3D12_RESOLVE_MODE_MAX";
				break;
			case D3D12_RESOLVE_MODE_AVERAGE:
				static_cast<_Mybase&>(*this) = "D3D12_RESOLVE_MODE_AVERAGE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESOLVE_MODE)]");
			};
		}

		enum_string(D3D12_VIEW_INSTANCING_FLAGS _Flags) {
			if (_Flags & D3D12_VIEW_INSTANCING_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_VIEW_INSTANCING_FLAG_NONE|";
			if (_Flags & D3D12_VIEW_INSTANCING_FLAG_ENABLE_VIEW_INSTANCE_MASKING)
				static_cast<_Mybase&>(*this) += "D3D12_VIEW_INSTANCING_FLAG_ENABLE_VIEW_INSTANCE_MASKING|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_VIEW_INSTANCING_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_VIEW_INSTANCING_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_SHADER_COMPONENT_MAPPING _Flags) {
			switch (_Flags) {
			case D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0";
				break;
			case D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1";
				break;
			case D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2";
				break;
			case D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3";
				break;
			case D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0";
				break;
			case D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_SHADER_COMPONENT_MAPPING)]");
			};
		}

		enum_string(D3D12_BUFFER_SRV_FLAGS _Flags) {
			if (_Flags & D3D12_BUFFER_SRV_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_BUFFER_SRV_FLAG_NONE|";
			if (_Flags & D3D12_BUFFER_SRV_FLAG_RAW)
				static_cast<_Mybase&>(*this) += "D3D12_BUFFER_SRV_FLAG_RAW|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_BUFFER_SRV_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_BUFFER_SRV_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_SRV_DIMENSION _Flags) {
			switch (_Flags) {
			case D3D12_SRV_DIMENSION_UNKNOWN:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_UNKNOWN";
				break;
			case D3D12_SRV_DIMENSION_BUFFER:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_BUFFER";
				break;
			case D3D12_SRV_DIMENSION_TEXTURE1D:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_TEXTURE1D";
				break;
			case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_TEXTURE1DARRAY";
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2D:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_TEXTURE2D";
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_TEXTURE2DARRAY";
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DMS:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_TEXTURE2DMS";
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY";
				break;
			case D3D12_SRV_DIMENSION_TEXTURE3D:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_TEXTURE3D";
				break;
			case D3D12_SRV_DIMENSION_TEXTURECUBE:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_TEXTURECUBE";
				break;
			case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_TEXTURECUBEARRAY";
				break;
			case D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE:
				static_cast<_Mybase&>(*this) = "D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_SRV_DIMENSION)]");
			};
		}

		enum_string(D3D12_FILTER _Flags) {
			if (_Flags & D3D12_FILTER_MIN_MAG_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MIN_MAG_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MIN_MAG_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MIN_MAG_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_ANISOTROPIC)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_ANISOTROPIC|";
			if (_Flags & D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT|";
			if (_Flags & D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT|";
			if (_Flags & D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT|";
			if (_Flags & D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT|";
			if (_Flags & D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_COMPARISON_ANISOTROPIC)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_COMPARISON_ANISOTROPIC|";
			if (_Flags & D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MINIMUM_ANISOTROPIC)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MINIMUM_ANISOTROPIC|";
			if (_Flags & D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT|";
			if (_Flags & D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR|";
			if (_Flags & D3D12_FILTER_MAXIMUM_ANISOTROPIC)
				static_cast<_Mybase&>(*this) += "D3D12_FILTER_MAXIMUM_ANISOTROPIC|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_FILTER_MIN_MAG_MIP_POINT";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_FILTER)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_FILTER_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_FILTER_TYPE_POINT:
				static_cast<_Mybase&>(*this) = "D3D12_FILTER_TYPE_POINT";
				break;
			case D3D12_FILTER_TYPE_LINEAR:
				static_cast<_Mybase&>(*this) = "D3D12_FILTER_TYPE_LINEAR";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_FILTER_TYPE)]");
			};
		}

		enum_string(D3D12_FILTER_REDUCTION_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_FILTER_REDUCTION_TYPE_STANDARD:
				static_cast<_Mybase&>(*this) = "D3D12_FILTER_REDUCTION_TYPE_STANDARD";
				break;
			case D3D12_FILTER_REDUCTION_TYPE_COMPARISON:
				static_cast<_Mybase&>(*this) = "D3D12_FILTER_REDUCTION_TYPE_COMPARISON";
				break;
			case D3D12_FILTER_REDUCTION_TYPE_MINIMUM:
				static_cast<_Mybase&>(*this) = "D3D12_FILTER_REDUCTION_TYPE_MINIMUM";
				break;
			case D3D12_FILTER_REDUCTION_TYPE_MAXIMUM:
				static_cast<_Mybase&>(*this) = "D3D12_FILTER_REDUCTION_TYPE_MAXIMUM";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_FILTER_REDUCTION_TYPE)]");
			};
		}

		enum_string(D3D12_TEXTURE_ADDRESS_MODE _Flags) {
			switch (_Flags) {
			case D3D12_TEXTURE_ADDRESS_MODE_WRAP:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_ADDRESS_MODE_WRAP";
				break;
			case D3D12_TEXTURE_ADDRESS_MODE_MIRROR:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_ADDRESS_MODE_MIRROR";
				break;
			case D3D12_TEXTURE_ADDRESS_MODE_CLAMP:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_ADDRESS_MODE_CLAMP";
				break;
			case D3D12_TEXTURE_ADDRESS_MODE_BORDER:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_ADDRESS_MODE_BORDER";
				break;
			case D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE:
				static_cast<_Mybase&>(*this) = "D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_TEXTURE_ADDRESS_MODE)]");
			};
		}

		enum_string(D3D12_BUFFER_UAV_FLAGS _Flags) {
			if (_Flags & D3D12_BUFFER_UAV_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_BUFFER_UAV_FLAG_NONE|";
			if (_Flags & D3D12_BUFFER_UAV_FLAG_RAW)
				static_cast<_Mybase&>(*this) += "D3D12_BUFFER_UAV_FLAG_RAW|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_BUFFER_UAV_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_BUFFER_UAV_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_UAV_DIMENSION _Flags) {
			switch (_Flags) {
			case D3D12_UAV_DIMENSION_UNKNOWN:
				static_cast<_Mybase&>(*this) = "D3D12_UAV_DIMENSION_UNKNOWN";
				break;
			case D3D12_UAV_DIMENSION_BUFFER:
				static_cast<_Mybase&>(*this) = "D3D12_UAV_DIMENSION_BUFFER";
				break;
			case D3D12_UAV_DIMENSION_TEXTURE1D:
				static_cast<_Mybase&>(*this) = "D3D12_UAV_DIMENSION_TEXTURE1D";
				break;
			case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_UAV_DIMENSION_TEXTURE1DARRAY";
				break;
			case D3D12_UAV_DIMENSION_TEXTURE2D:
				static_cast<_Mybase&>(*this) = "D3D12_UAV_DIMENSION_TEXTURE2D";
				break;
			case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_UAV_DIMENSION_TEXTURE2DARRAY";
				break;
			case D3D12_UAV_DIMENSION_TEXTURE3D:
				static_cast<_Mybase&>(*this) = "D3D12_UAV_DIMENSION_TEXTURE3D";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_UAV_DIMENSION)]");
			};
		}

		enum_string(D3D12_RTV_DIMENSION _Flags) {
			switch (_Flags) {
			case D3D12_RTV_DIMENSION_UNKNOWN:
				static_cast<_Mybase&>(*this) = "D3D12_RTV_DIMENSION_UNKNOWN";
				break;
			case D3D12_RTV_DIMENSION_BUFFER:
				static_cast<_Mybase&>(*this) = "D3D12_RTV_DIMENSION_BUFFER";
				break;
			case D3D12_RTV_DIMENSION_TEXTURE1D:
				static_cast<_Mybase&>(*this) = "D3D12_RTV_DIMENSION_TEXTURE1D";
				break;
			case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_RTV_DIMENSION_TEXTURE1DARRAY";
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2D:
				static_cast<_Mybase&>(*this) = "D3D12_RTV_DIMENSION_TEXTURE2D";
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_RTV_DIMENSION_TEXTURE2DARRAY";
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2DMS:
				static_cast<_Mybase&>(*this) = "D3D12_RTV_DIMENSION_TEXTURE2DMS";
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY";
				break;
			case D3D12_RTV_DIMENSION_TEXTURE3D:
				static_cast<_Mybase&>(*this) = "D3D12_RTV_DIMENSION_TEXTURE3D";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RTV_DIMENSION)]");
			};
		}

		enum_string(D3D12_DSV_FLAGS _Flags) {
			if (_Flags & D3D12_DSV_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_DSV_FLAG_NONE|";
			if (_Flags & D3D12_DSV_FLAG_READ_ONLY_DEPTH)
				static_cast<_Mybase&>(*this) += "D3D12_DSV_FLAG_READ_ONLY_DEPTH|";
			if (_Flags & D3D12_DSV_FLAG_READ_ONLY_STENCIL)
				static_cast<_Mybase&>(*this) += "D3D12_DSV_FLAG_READ_ONLY_STENCIL|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_DSV_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DSV_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_DSV_DIMENSION _Flags) {
			switch (_Flags) {
			case D3D12_DSV_DIMENSION_UNKNOWN:
				static_cast<_Mybase&>(*this) = "D3D12_DSV_DIMENSION_UNKNOWN";
				break;
			case D3D12_DSV_DIMENSION_TEXTURE1D:
				static_cast<_Mybase&>(*this) = "D3D12_DSV_DIMENSION_TEXTURE1D";
				break;
			case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_DSV_DIMENSION_TEXTURE1DARRAY";
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2D:
				static_cast<_Mybase&>(*this) = "D3D12_DSV_DIMENSION_TEXTURE2D";
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_DSV_DIMENSION_TEXTURE2DARRAY";
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2DMS:
				static_cast<_Mybase&>(*this) = "D3D12_DSV_DIMENSION_TEXTURE2DMS";
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
				static_cast<_Mybase&>(*this) = "D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DSV_DIMENSION)]");
			};
		}

		enum_string(D3D12_CLEAR_FLAGS _Flags) {
			if (_Flags & D3D12_CLEAR_FLAG_DEPTH)
				static_cast<_Mybase&>(*this) += "D3D12_CLEAR_FLAG_DEPTH|";
			if (_Flags & D3D12_CLEAR_FLAG_STENCIL)
				static_cast<_Mybase&>(*this) += "D3D12_CLEAR_FLAG_STENCIL|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_CLEAR_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_FENCE_FLAGS _Flags) {
			if (_Flags & D3D12_FENCE_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_FENCE_FLAG_NONE|";
			if (_Flags & D3D12_FENCE_FLAG_SHARED)
				static_cast<_Mybase&>(*this) += "D3D12_FENCE_FLAG_SHARED|";
			if (_Flags & D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER)
				static_cast<_Mybase&>(*this) += "D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER|";
			if (_Flags & D3D12_FENCE_FLAG_NON_MONITORED)
				static_cast<_Mybase&>(*this) += "D3D12_FENCE_FLAG_NON_MONITORED|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_FENCE_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_FENCE_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_DESCRIPTOR_HEAP_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
				static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV";
				break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
				static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER";
				break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
				static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_HEAP_TYPE_RTV";
				break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
				static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_HEAP_TYPE_DSV";
				break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES:
				static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DESCRIPTOR_HEAP_TYPE)]");
			};
		}

		enum_string(D3D12_DESCRIPTOR_HEAP_FLAGS _Flags) {
			if (_Flags & D3D12_DESCRIPTOR_HEAP_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_DESCRIPTOR_HEAP_FLAG_NONE|";
			if (_Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
				static_cast<_Mybase&>(*this) += "D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_HEAP_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DESCRIPTOR_HEAP_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_DESCRIPTOR_RANGE_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
				static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_RANGE_TYPE_SRV";
				break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
				static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_RANGE_TYPE_UAV";
				break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
				static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_RANGE_TYPE_CBV";
				break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
				static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DESCRIPTOR_RANGE_TYPE)]");
			};
		}

		enum_string(D3D12_SHADER_VISIBILITY _Flags) {
			switch (_Flags) {
			case D3D12_SHADER_VISIBILITY_ALL:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_VISIBILITY_ALL";
				break;
			case D3D12_SHADER_VISIBILITY_VERTEX:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_VISIBILITY_VERTEX";
				break;
			case D3D12_SHADER_VISIBILITY_HULL:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_VISIBILITY_HULL";
				break;
			case D3D12_SHADER_VISIBILITY_DOMAIN:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_VISIBILITY_DOMAIN";
				break;
			case D3D12_SHADER_VISIBILITY_GEOMETRY:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_VISIBILITY_GEOMETRY";
				break;
			case D3D12_SHADER_VISIBILITY_PIXEL:
				static_cast<_Mybase&>(*this) = "D3D12_SHADER_VISIBILITY_PIXEL";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_SHADER_VISIBILITY)]");
			};
		}

		enum_string(D3D12_ROOT_PARAMETER_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
				static_cast<_Mybase&>(*this) = "D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE";
				break;
			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
				static_cast<_Mybase&>(*this) = "D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS";
				break;
			case D3D12_ROOT_PARAMETER_TYPE_CBV:
				static_cast<_Mybase&>(*this) = "D3D12_ROOT_PARAMETER_TYPE_CBV";
				break;
			case D3D12_ROOT_PARAMETER_TYPE_SRV:
				static_cast<_Mybase&>(*this) = "D3D12_ROOT_PARAMETER_TYPE_SRV";
				break;
			case D3D12_ROOT_PARAMETER_TYPE_UAV:
				static_cast<_Mybase&>(*this) = "D3D12_ROOT_PARAMETER_TYPE_UAV";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_ROOT_PARAMETER_TYPE)]");
			};
		}

		enum_string(D3D12_ROOT_SIGNATURE_FLAGS _Flags) {
			if (_Flags & D3D12_ROOT_SIGNATURE_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_SIGNATURE_FLAG_NONE|";
			if (_Flags & D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT|";
			if (_Flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS|";
			if (_Flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS|";
			if (_Flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS|";
			if (_Flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS|";
			if (_Flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS|";
			if (_Flags & D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT|";
			if (_Flags & D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_ROOT_SIGNATURE_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_ROOT_SIGNATURE_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_STATIC_BORDER_COLOR _Flags) {
			switch (_Flags) {
			case D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK:
				static_cast<_Mybase&>(*this) = "D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK";
				break;
			case D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK:
				static_cast<_Mybase&>(*this) = "D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK";
				break;
			case D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE:
				static_cast<_Mybase&>(*this) = "D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_STATIC_BORDER_COLOR)]");
			};
		}

		enum_string(D3D12_DESCRIPTOR_RANGE_FLAGS _Flags) {
			if (_Flags & D3D12_DESCRIPTOR_RANGE_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_DESCRIPTOR_RANGE_FLAG_NONE|";
			if (_Flags & D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE)
				static_cast<_Mybase&>(*this) += "D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE|";
			if (_Flags & D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE)
				static_cast<_Mybase&>(*this) += "D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE|";
			if (_Flags & D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE)
				static_cast<_Mybase&>(*this) += "D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE|";
			if (_Flags & D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC)
				static_cast<_Mybase&>(*this) += "D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC|";
			if (_Flags & D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_STATIC_KEEPING_BUFFER_BOUNDS_CHECKS)
				static_cast<_Mybase&>(*this) += "D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_STATIC_KEEPING_BUFFER_BOUNDS_CHECKS|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_DESCRIPTOR_RANGE_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DESCRIPTOR_RANGE_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_ROOT_DESCRIPTOR_FLAGS _Flags) {
			if (_Flags & D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_DESCRIPTOR_FLAG_NONE|";
			if (_Flags & D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE|";
			if (_Flags & D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE|";
			if (_Flags & D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC)
				static_cast<_Mybase&>(*this) += "D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_ROOT_DESCRIPTOR_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_ROOT_DESCRIPTOR_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_QUERY_HEAP_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_QUERY_HEAP_TYPE_OCCLUSION:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_HEAP_TYPE_OCCLUSION";
				break;
			case D3D12_QUERY_HEAP_TYPE_TIMESTAMP:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_HEAP_TYPE_TIMESTAMP";
				break;
			case D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS";
				break;
			case D3D12_QUERY_HEAP_TYPE_SO_STATISTICS:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_HEAP_TYPE_SO_STATISTICS";
				break;
			case D3D12_QUERY_HEAP_TYPE_VIDEO_DECODE_STATISTICS:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_HEAP_TYPE_VIDEO_DECODE_STATISTICS";
				break;
			case D3D12_QUERY_HEAP_TYPE_COPY_QUEUE_TIMESTAMP:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_HEAP_TYPE_COPY_QUEUE_TIMESTAMP";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_QUERY_HEAP_TYPE)]");
			};
		}

		enum_string(D3D12_QUERY_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_QUERY_TYPE_OCCLUSION:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_TYPE_OCCLUSION";
				break;
			case D3D12_QUERY_TYPE_BINARY_OCCLUSION:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_TYPE_BINARY_OCCLUSION";
				break;
			case D3D12_QUERY_TYPE_TIMESTAMP:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_TYPE_TIMESTAMP";
				break;
			case D3D12_QUERY_TYPE_PIPELINE_STATISTICS:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_TYPE_PIPELINE_STATISTICS";
				break;
			case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM0:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_TYPE_SO_STATISTICS_STREAM0";
				break;
			case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM1:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_TYPE_SO_STATISTICS_STREAM1";
				break;
			case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM2:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_TYPE_SO_STATISTICS_STREAM2";
				break;
			case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM3:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_TYPE_SO_STATISTICS_STREAM3";
				break;
			case D3D12_QUERY_TYPE_VIDEO_DECODE_STATISTICS:
				static_cast<_Mybase&>(*this) = "D3D12_QUERY_TYPE_VIDEO_DECODE_STATISTICS";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_QUERY_TYPE)]");
			};
		}

		enum_string(D3D12_PREDICATION_OP _Flags) {
			switch (_Flags) {
			case D3D12_PREDICATION_OP_EQUAL_ZERO:
				static_cast<_Mybase&>(*this) = "D3D12_PREDICATION_OP_EQUAL_ZERO";
				break;
			case D3D12_PREDICATION_OP_NOT_EQUAL_ZERO:
				static_cast<_Mybase&>(*this) = "D3D12_PREDICATION_OP_NOT_EQUAL_ZERO";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_PREDICATION_OP)]");
			};
		}

		enum_string(D3D12_INDIRECT_ARGUMENT_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW:
				static_cast<_Mybase&>(*this) = "D3D12_INDIRECT_ARGUMENT_TYPE_DRAW";
				break;
			case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED:
				static_cast<_Mybase&>(*this) = "D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED";
				break;
			case D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH:
				static_cast<_Mybase&>(*this) = "D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH";
				break;
			case D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW:
				static_cast<_Mybase&>(*this) = "D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW";
				break;
			case D3D12_INDIRECT_ARGUMENT_TYPE_INDEX_BUFFER_VIEW:
				static_cast<_Mybase&>(*this) = "D3D12_INDIRECT_ARGUMENT_TYPE_INDEX_BUFFER_VIEW";
				break;
			case D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT:
				static_cast<_Mybase&>(*this) = "D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT";
				break;
			case D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW:
				static_cast<_Mybase&>(*this) = "D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW";
				break;
			case D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW:
				static_cast<_Mybase&>(*this) = "D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW";
				break;
			case D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW:
				static_cast<_Mybase&>(*this) = "D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_INDIRECT_ARGUMENT_TYPE)]");
			};
		}

		enum_string(D3D12_WRITEBUFFERIMMEDIATE_MODE _Flags) {
			if (_Flags & D3D12_WRITEBUFFERIMMEDIATE_MODE_DEFAULT)
				static_cast<_Mybase&>(*this) += "D3D12_WRITEBUFFERIMMEDIATE_MODE_DEFAULT|";
			if (_Flags & D3D12_WRITEBUFFERIMMEDIATE_MODE_MARKER_IN)
				static_cast<_Mybase&>(*this) += "D3D12_WRITEBUFFERIMMEDIATE_MODE_MARKER_IN|";
			if (_Flags & D3D12_WRITEBUFFERIMMEDIATE_MODE_MARKER_OUT)
				static_cast<_Mybase&>(*this) += "D3D12_WRITEBUFFERIMMEDIATE_MODE_MARKER_OUT|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_WRITEBUFFERIMMEDIATE_MODE_DEFAULT";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_WRITEBUFFERIMMEDIATE_MODE)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_MULTIPLE_FENCE_WAIT_FLAGS _Flags) {
			if (_Flags & D3D12_MULTIPLE_FENCE_WAIT_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_MULTIPLE_FENCE_WAIT_FLAG_NONE|";
			if (_Flags & D3D12_MULTIPLE_FENCE_WAIT_FLAG_ANY)
				static_cast<_Mybase&>(*this) += "D3D12_MULTIPLE_FENCE_WAIT_FLAG_ANY|";
			if (_Flags & D3D12_MULTIPLE_FENCE_WAIT_FLAG_ALL)
				static_cast<_Mybase&>(*this) += "D3D12_MULTIPLE_FENCE_WAIT_FLAG_ALL|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_MULTIPLE_FENCE_WAIT_FLAG_ALL";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_MULTIPLE_FENCE_WAIT_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RESIDENCY_PRIORITY _Flags) {
			if (_Flags & D3D12_RESIDENCY_PRIORITY_MINIMUM)
				static_cast<_Mybase&>(*this) += "D3D12_RESIDENCY_PRIORITY_MINIMUM|";
			if (_Flags & D3D12_RESIDENCY_PRIORITY_LOW)
				static_cast<_Mybase&>(*this) += "D3D12_RESIDENCY_PRIORITY_LOW|";
			if (_Flags & D3D12_RESIDENCY_PRIORITY_NORMAL)
				static_cast<_Mybase&>(*this) += "D3D12_RESIDENCY_PRIORITY_NORMAL|";
			if (_Flags & D3D12_RESIDENCY_PRIORITY_HIGH)
				static_cast<_Mybase&>(*this) += "D3D12_RESIDENCY_PRIORITY_HIGH|";
			if (_Flags & D3D12_RESIDENCY_PRIORITY_MAXIMUM)
				static_cast<_Mybase&>(*this) += "D3D12_RESIDENCY_PRIORITY_MAXIMUM|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESIDENCY_PRIORITY)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RESIDENCY_FLAGS _Flags) {
			if (_Flags & D3D12_RESIDENCY_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_RESIDENCY_FLAG_NONE|";
			if (_Flags & D3D12_RESIDENCY_FLAG_DENY_OVERBUDGET)
				static_cast<_Mybase&>(*this) += "D3D12_RESIDENCY_FLAG_DENY_OVERBUDGET|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RESIDENCY_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RESIDENCY_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_COMMAND_LIST_FLAGS _Flags) {
			switch (_Flags) {
			case D3D12_COMMAND_LIST_FLAG_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_LIST_FLAG_NONE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_COMMAND_LIST_FLAGS)]");
			};
		}

		enum_string(D3D12_COMMAND_POOL_FLAGS _Flags) {
			switch (_Flags) {
			case D3D12_COMMAND_POOL_FLAG_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_POOL_FLAG_NONE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_COMMAND_POOL_FLAGS)]");
			};
		}

		enum_string(D3D12_COMMAND_RECORDER_FLAGS _Flags) {
			switch (_Flags) {
			case D3D12_COMMAND_RECORDER_FLAG_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_COMMAND_RECORDER_FLAG_NONE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_COMMAND_RECORDER_FLAGS)]");
			};
		}

		enum_string(D3D12_PROTECTED_SESSION_STATUS _Flags) {
			switch (_Flags) {
			case D3D12_PROTECTED_SESSION_STATUS_OK:
				static_cast<_Mybase&>(*this) = "D3D12_PROTECTED_SESSION_STATUS_OK";
				break;
			case D3D12_PROTECTED_SESSION_STATUS_INVALID:
				static_cast<_Mybase&>(*this) = "D3D12_PROTECTED_SESSION_STATUS_INVALID";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_PROTECTED_SESSION_STATUS)]");
			};
		}

		enum_string(D3D12_PROTECTED_RESOURCE_SESSION_SUPPORT_FLAGS _Flags) {
			if (_Flags & D3D12_PROTECTED_RESOURCE_SESSION_SUPPORT_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_PROTECTED_RESOURCE_SESSION_SUPPORT_FLAG_NONE|";
			if (_Flags & D3D12_PROTECTED_RESOURCE_SESSION_SUPPORT_FLAG_SUPPORTED)
				static_cast<_Mybase&>(*this) += "D3D12_PROTECTED_RESOURCE_SESSION_SUPPORT_FLAG_SUPPORTED|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_PROTECTED_RESOURCE_SESSION_SUPPORT_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_PROTECTED_RESOURCE_SESSION_SUPPORT_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_PROTECTED_RESOURCE_SESSION_FLAGS _Flags) {
			switch (_Flags) {
			case D3D12_PROTECTED_RESOURCE_SESSION_FLAG_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_PROTECTED_RESOURCE_SESSION_FLAG_NONE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_PROTECTED_RESOURCE_SESSION_FLAGS)]");
			};
		}

		enum_string(D3D12_LIFETIME_STATE _Flags) {
			switch (_Flags) {
			case D3D12_LIFETIME_STATE_IN_USE:
				static_cast<_Mybase&>(*this) = "D3D12_LIFETIME_STATE_IN_USE";
				break;
			case D3D12_LIFETIME_STATE_NOT_IN_USE:
				static_cast<_Mybase&>(*this) = "D3D12_LIFETIME_STATE_NOT_IN_USE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_LIFETIME_STATE)]");
			};
		}

		enum_string(D3D12_META_COMMAND_PARAMETER_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_META_COMMAND_PARAMETER_TYPE_FLOAT:
				static_cast<_Mybase&>(*this) = "D3D12_META_COMMAND_PARAMETER_TYPE_FLOAT";
				break;
			case D3D12_META_COMMAND_PARAMETER_TYPE_UINT64:
				static_cast<_Mybase&>(*this) = "D3D12_META_COMMAND_PARAMETER_TYPE_UINT64";
				break;
			case D3D12_META_COMMAND_PARAMETER_TYPE_GPU_VIRTUAL_ADDRESS:
				static_cast<_Mybase&>(*this) = "D3D12_META_COMMAND_PARAMETER_TYPE_GPU_VIRTUAL_ADDRESS";
				break;
			case D3D12_META_COMMAND_PARAMETER_TYPE_CPU_DESCRIPTOR_HANDLE_HEAP_TYPE_CBV_SRV_UAV:
				static_cast<_Mybase&>(*this) = "D3D12_META_COMMAND_PARAMETER_TYPE_CPU_DESCRIPTOR_HANDLE_HEAP_TYPE_CBV_SRV_UAV";
				break;
			case D3D12_META_COMMAND_PARAMETER_TYPE_GPU_DESCRIPTOR_HANDLE_HEAP_TYPE_CBV_SRV_UAV:
				static_cast<_Mybase&>(*this) = "D3D12_META_COMMAND_PARAMETER_TYPE_GPU_DESCRIPTOR_HANDLE_HEAP_TYPE_CBV_SRV_UAV";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_META_COMMAND_PARAMETER_TYPE)]");
			};
		}

		enum_string(D3D12_META_COMMAND_PARAMETER_FLAGS _Flags) {
			if (_Flags & D3D12_META_COMMAND_PARAMETER_FLAG_INPUT)
				static_cast<_Mybase&>(*this) += "D3D12_META_COMMAND_PARAMETER_FLAG_INPUT|";
			if (_Flags & D3D12_META_COMMAND_PARAMETER_FLAG_OUTPUT)
				static_cast<_Mybase&>(*this) += "D3D12_META_COMMAND_PARAMETER_FLAG_OUTPUT|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_META_COMMAND_PARAMETER_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_META_COMMAND_PARAMETER_STAGE _Flags) {
			switch (_Flags) {
			case D3D12_META_COMMAND_PARAMETER_STAGE_CREATION:
				static_cast<_Mybase&>(*this) = "D3D12_META_COMMAND_PARAMETER_STAGE_CREATION";
				break;
			case D3D12_META_COMMAND_PARAMETER_STAGE_INITIALIZATION:
				static_cast<_Mybase&>(*this) = "D3D12_META_COMMAND_PARAMETER_STAGE_INITIALIZATION";
				break;
			case D3D12_META_COMMAND_PARAMETER_STAGE_EXECUTION:
				static_cast<_Mybase&>(*this) = "D3D12_META_COMMAND_PARAMETER_STAGE_EXECUTION";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_META_COMMAND_PARAMETER_STAGE)]");
			};
		}

		enum_string(D3D12_GRAPHICS_STATES _Flags) {
			switch (_Flags) {
			case D3D12_GRAPHICS_STATE_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_NONE";
				break;
			case D3D12_GRAPHICS_STATE_IA_VERTEX_BUFFERS:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_IA_VERTEX_BUFFERS";
				break;
			case D3D12_GRAPHICS_STATE_IA_INDEX_BUFFER:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_IA_INDEX_BUFFER";
				break;
			case D3D12_GRAPHICS_STATE_IA_PRIMITIVE_TOPOLOGY:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_IA_PRIMITIVE_TOPOLOGY";
				break;
			case D3D12_GRAPHICS_STATE_DESCRIPTOR_HEAP:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_DESCRIPTOR_HEAP";
				break;
			case D3D12_GRAPHICS_STATE_GRAPHICS_ROOT_SIGNATURE:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_GRAPHICS_ROOT_SIGNATURE";
				break;
			case D3D12_GRAPHICS_STATE_COMPUTE_ROOT_SIGNATURE:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_COMPUTE_ROOT_SIGNATURE";
				break;
			case D3D12_GRAPHICS_STATE_RS_VIEWPORTS:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_RS_VIEWPORTS";
				break;
			case D3D12_GRAPHICS_STATE_RS_SCISSOR_RECTS:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_RS_SCISSOR_RECTS";
				break;
			case D3D12_GRAPHICS_STATE_PREDICATION:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_PREDICATION";
				break;
			case D3D12_GRAPHICS_STATE_OM_RENDER_TARGETS:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_OM_RENDER_TARGETS";
				break;
			case D3D12_GRAPHICS_STATE_OM_STENCIL_REF:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_OM_STENCIL_REF";
				break;
			case D3D12_GRAPHICS_STATE_OM_BLEND_FACTOR:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_OM_BLEND_FACTOR";
				break;
			case D3D12_GRAPHICS_STATE_PIPELINE_STATE:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_PIPELINE_STATE";
				break;
			case D3D12_GRAPHICS_STATE_SO_TARGETS:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_SO_TARGETS";
				break;
			case D3D12_GRAPHICS_STATE_OM_DEPTH_BOUNDS:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_OM_DEPTH_BOUNDS";
				break;
			case D3D12_GRAPHICS_STATE_SAMPLE_POSITIONS:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_SAMPLE_POSITIONS";
				break;
			case D3D12_GRAPHICS_STATE_VIEW_INSTANCE_MASK:
				static_cast<_Mybase&>(*this) = "D3D12_GRAPHICS_STATE_VIEW_INSTANCE_MASK";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_GRAPHICS_STATES)]");
			};
		}

		enum_string(D3D12_STATE_SUBOBJECT_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_STATE_SUBOBJECT_TYPE_STATE_OBJECT_CONFIG:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_STATE_OBJECT_CONFIG";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_MAX_VALID:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_SUBOBJECT_TYPE_MAX_VALID";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_STATE_SUBOBJECT_TYPE)]");
			};
		}

		enum_string(D3D12_STATE_OBJECT_FLAGS _Flags) {
			if (_Flags & D3D12_STATE_OBJECT_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_STATE_OBJECT_FLAG_NONE|";
			if (_Flags & D3D12_STATE_OBJECT_FLAG_ALLOW_LOCAL_DEPENDENCIES_ON_EXTERNAL_DEFINITIONS)
				static_cast<_Mybase&>(*this) += "D3D12_STATE_OBJECT_FLAG_ALLOW_LOCAL_DEPENDENCIES_ON_EXTERNAL_DEFINITIONS|";
			if (_Flags & D3D12_STATE_OBJECT_FLAG_ALLOW_EXTERNAL_DEPENDENCIES_ON_LOCAL_DEFINITIONS)
				static_cast<_Mybase&>(*this) += "D3D12_STATE_OBJECT_FLAG_ALLOW_EXTERNAL_DEPENDENCIES_ON_LOCAL_DEFINITIONS|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_STATE_OBJECT_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_STATE_OBJECT_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_EXPORT_FLAGS _Flags) {
			switch (_Flags) {
			case D3D12_EXPORT_FLAG_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_EXPORT_FLAG_NONE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_EXPORT_FLAGS)]");
			};
		}

		enum_string(D3D12_HIT_GROUP_TYPE _Flags) {
			if (_Flags & D3D12_HIT_GROUP_TYPE_TRIANGLES)
				static_cast<_Mybase&>(*this) += "D3D12_HIT_GROUP_TYPE_TRIANGLES|";
			if (_Flags & D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE)
				static_cast<_Mybase&>(*this) += "D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_HIT_GROUP_TYPE_TRIANGLES";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_HIT_GROUP_TYPE)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_STATE_OBJECT_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_STATE_OBJECT_TYPE_COLLECTION:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_OBJECT_TYPE_COLLECTION";
				break;
			case D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE:
				static_cast<_Mybase&>(*this) = "D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_STATE_OBJECT_TYPE)]");
			};
		}

		enum_string(D3D12_RAYTRACING_GEOMETRY_FLAGS _Flags) {
			if (_Flags & D3D12_RAYTRACING_GEOMETRY_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_GEOMETRY_FLAG_NONE|";
			if (_Flags & D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE|";
			if (_Flags & D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_GEOMETRY_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RAYTRACING_GEOMETRY_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RAYTRACING_GEOMETRY_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES:
				static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES";
				break;
			case D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS:
				static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RAYTRACING_GEOMETRY_TYPE)]");
			};
		}

		enum_string(D3D12_RAYTRACING_INSTANCE_FLAGS _Flags) {
			if (_Flags & D3D12_RAYTRACING_INSTANCE_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_INSTANCE_FLAG_NONE|";
			if (_Flags & D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE|";
			if (_Flags & D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE|";
			if (_Flags & D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE|";
			if (_Flags & D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_NON_OPAQUE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_NON_OPAQUE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_INSTANCE_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RAYTRACING_INSTANCE_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS _Flags) {
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE _Flags) {
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_CLONE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_CLONE|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_COMPACT)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_COMPACT|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_VISUALIZATION_DECODE_FOR_TOOLS)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_VISUALIZATION_DECODE_FOR_TOOLS|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_SERIALIZE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_SERIALIZE|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_DESERIALIZE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_DESERIALIZE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_CLONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE _Flags) {
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_ELEMENTS_LAYOUT _Flags) {
			if (_Flags & D3D12_ELEMENTS_LAYOUT_ARRAY)
				static_cast<_Mybase&>(*this) += "D3D12_ELEMENTS_LAYOUT_ARRAY|";
			if (_Flags & D3D12_ELEMENTS_LAYOUT_ARRAY_OF_POINTERS)
				static_cast<_Mybase&>(*this) += "D3D12_ELEMENTS_LAYOUT_ARRAY_OF_POINTERS|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_ELEMENTS_LAYOUT_ARRAY";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_ELEMENTS_LAYOUT)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_TYPE _Flags) {
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_TOOLS_VISUALIZATION)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_TOOLS_VISUALIZATION|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_SERIALIZATION)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_SERIALIZATION|";
			if (_Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_CURRENT_SIZE)
				static_cast<_Mybase&>(*this) += "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_CURRENT_SIZE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_TYPE)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_SERIALIZED_DATA_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_SERIALIZED_DATA_RAYTRACING_ACCELERATION_STRUCTURE:
				static_cast<_Mybase&>(*this) = "D3D12_SERIALIZED_DATA_RAYTRACING_ACCELERATION_STRUCTURE";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_SERIALIZED_DATA_TYPE)]");
			};
		}

		enum_string(D3D12_DRIVER_MATCHING_IDENTIFIER_STATUS _Flags) {
			if (_Flags & D3D12_DRIVER_MATCHING_IDENTIFIER_COMPATIBLE_WITH_DEVICE)
				static_cast<_Mybase&>(*this) += "D3D12_DRIVER_MATCHING_IDENTIFIER_COMPATIBLE_WITH_DEVICE|";
			if (_Flags & D3D12_DRIVER_MATCHING_IDENTIFIER_UNSUPPORTED_TYPE)
				static_cast<_Mybase&>(*this) += "D3D12_DRIVER_MATCHING_IDENTIFIER_UNSUPPORTED_TYPE|";
			if (_Flags & D3D12_DRIVER_MATCHING_IDENTIFIER_UNRECOGNIZED)
				static_cast<_Mybase&>(*this) += "D3D12_DRIVER_MATCHING_IDENTIFIER_UNRECOGNIZED|";
			if (_Flags & D3D12_DRIVER_MATCHING_IDENTIFIER_INCOMPATIBLE_VERSION)
				static_cast<_Mybase&>(*this) += "D3D12_DRIVER_MATCHING_IDENTIFIER_INCOMPATIBLE_VERSION|";
			if (_Flags & D3D12_DRIVER_MATCHING_IDENTIFIER_INCOMPATIBLE_TYPE)
				static_cast<_Mybase&>(*this) += "D3D12_DRIVER_MATCHING_IDENTIFIER_INCOMPATIBLE_TYPE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_DRIVER_MATCHING_IDENTIFIER_COMPATIBLE_WITH_DEVICE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DRIVER_MATCHING_IDENTIFIER_STATUS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_RAY_FLAGS _Flags) {
			if (_Flags & D3D12_RAY_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_RAY_FLAG_NONE|";
			if (_Flags & D3D12_RAY_FLAG_FORCE_OPAQUE)
				static_cast<_Mybase&>(*this) += "D3D12_RAY_FLAG_FORCE_OPAQUE|";
			if (_Flags & D3D12_RAY_FLAG_FORCE_NON_OPAQUE)
				static_cast<_Mybase&>(*this) += "D3D12_RAY_FLAG_FORCE_NON_OPAQUE|";
			if (_Flags & D3D12_RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH)
				static_cast<_Mybase&>(*this) += "D3D12_RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH|";
			if (_Flags & D3D12_RAY_FLAG_SKIP_CLOSEST_HIT_SHADER)
				static_cast<_Mybase&>(*this) += "D3D12_RAY_FLAG_SKIP_CLOSEST_HIT_SHADER|";
			if (_Flags & D3D12_RAY_FLAG_CULL_BACK_FACING_TRIANGLES)
				static_cast<_Mybase&>(*this) += "D3D12_RAY_FLAG_CULL_BACK_FACING_TRIANGLES|";
			if (_Flags & D3D12_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES)
				static_cast<_Mybase&>(*this) += "D3D12_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES|";
			if (_Flags & D3D12_RAY_FLAG_CULL_OPAQUE)
				static_cast<_Mybase&>(*this) += "D3D12_RAY_FLAG_CULL_OPAQUE|";
			if (_Flags & D3D12_RAY_FLAG_CULL_NON_OPAQUE)
				static_cast<_Mybase&>(*this) += "D3D12_RAY_FLAG_CULL_NON_OPAQUE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RAY_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RAY_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_HIT_KIND _Flags) {
			if (_Flags & D3D12_HIT_KIND_TRIANGLE_FRONT_FACE)
				static_cast<_Mybase&>(*this) += "D3D12_HIT_KIND_TRIANGLE_FRONT_FACE|";
			if (_Flags & D3D12_HIT_KIND_TRIANGLE_BACK_FACE)
				static_cast<_Mybase&>(*this) += "D3D12_HIT_KIND_TRIANGLE_BACK_FACE|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_HIT_KIND)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_AUTO_BREADCRUMB_OP _Flags) {
			switch (_Flags) {
			case D3D12_AUTO_BREADCRUMB_OP_SETMARKER:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_SETMARKER";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_BEGINEVENT:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_BEGINEVENT";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_ENDEVENT:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_ENDEVENT";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_DRAWINSTANCED:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_DRAWINSTANCED";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_DRAWINDEXEDINSTANCED:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_DRAWINDEXEDINSTANCED";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_EXECUTEINDIRECT:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_EXECUTEINDIRECT";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_DISPATCH:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_DISPATCH";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_COPYBUFFERREGION:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_COPYBUFFERREGION";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_COPYTEXTUREREGION:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_COPYTEXTUREREGION";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_COPYRESOURCE:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_COPYRESOURCE";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_COPYTILES:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_COPYTILES";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCE:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCE";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_CLEARRENDERTARGETVIEW:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_CLEARRENDERTARGETVIEW";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_CLEARUNORDEREDACCESSVIEW:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_CLEARUNORDEREDACCESSVIEW";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_CLEARDEPTHSTENCILVIEW:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_CLEARDEPTHSTENCILVIEW";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_RESOURCEBARRIER:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_RESOURCEBARRIER";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_EXECUTEBUNDLE:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_EXECUTEBUNDLE";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_PRESENT:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_PRESENT";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_RESOLVEQUERYDATA:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_RESOLVEQUERYDATA";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_ENDSUBMISSION:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_ENDSUBMISSION";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT64:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT64";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCEREGION:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCEREGION";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_WRITEBUFFERIMMEDIATE:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_WRITEBUFFERIMMEDIATE";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME1:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME1";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_SETPROTECTEDRESOURCESESSION:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_SETPROTECTEDRESOURCESESSION";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME2:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME2";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES1:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES1";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_BUILDRAYTRACINGACCELERATIONSTRUCTURE:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_BUILDRAYTRACINGACCELERATIONSTRUCTURE";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_EMITRAYTRACINGACCELERATIONSTRUCTUREPOSTBUILDINFO:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_EMITRAYTRACINGACCELERATIONSTRUCTUREPOSTBUILDINFO";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_COPYRAYTRACINGACCELERATIONSTRUCTURE:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_COPYRAYTRACINGACCELERATIONSTRUCTURE";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_DISPATCHRAYS:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_DISPATCHRAYS";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_INITIALIZEMETACOMMAND:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_INITIALIZEMETACOMMAND";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_EXECUTEMETACOMMAND:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_EXECUTEMETACOMMAND";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_ESTIMATEMOTION:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_ESTIMATEMOTION";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_RESOLVEMOTIONVECTORHEAP:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_RESOLVEMOTIONVECTORHEAP";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_SETPIPELINESTATE1:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_SETPIPELINESTATE1";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_INITIALIZEEXTENSIONCOMMAND:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_INITIALIZEEXTENSIONCOMMAND";
				break;
			case D3D12_AUTO_BREADCRUMB_OP_EXECUTEEXTENSIONCOMMAND:
				static_cast<_Mybase&>(*this) = "D3D12_AUTO_BREADCRUMB_OP_EXECUTEEXTENSIONCOMMAND";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_AUTO_BREADCRUMB_OP)]");
			};
		}

		enum_string(D3D12_DRED_VERSION _Flags) {
			if (_Flags & D3D12_DRED_VERSION_1_0)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_VERSION_1_0|";
			if (_Flags & D3D12_DRED_VERSION_1_1)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_VERSION_1_1|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DRED_VERSION)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_DRED_FLAGS _Flags) {
			switch (_Flags) {
			case D3D12_DRED_FLAG_NONE:
				static_cast<_Mybase&>(*this) = "D3D12_DRED_FLAG_NONE";
				break;
			case D3D12_DRED_FLAG_FORCE_ENABLE:
				static_cast<_Mybase&>(*this) = "D3D12_DRED_FLAG_FORCE_ENABLE";
				break;
			case D3D12_DRED_FLAG_DISABLE_AUTOBREADCRUMBS:
				static_cast<_Mybase&>(*this) = "D3D12_DRED_FLAG_DISABLE_AUTOBREADCRUMBS";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DRED_FLAGS)]");
			};
		}

		enum_string(D3D12_DRED_ENABLEMENT _Flags) {
			switch (_Flags) {
			case D3D12_DRED_ENABLEMENT_SYSTEM_CONTROLLED:
				static_cast<_Mybase&>(*this) = "D3D12_DRED_ENABLEMENT_SYSTEM_CONTROLLED";
				break;
			case D3D12_DRED_ENABLEMENT_FORCED_OFF:
				static_cast<_Mybase&>(*this) = "D3D12_DRED_ENABLEMENT_FORCED_OFF";
				break;
			case D3D12_DRED_ENABLEMENT_FORCED_ON:
				static_cast<_Mybase&>(*this) = "D3D12_DRED_ENABLEMENT_FORCED_ON";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DRED_ENABLEMENT)]");
			};
		}

		enum_string(D3D12_DRED_ALLOCATION_TYPE _Flags) {
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_COMMAND_QUEUE)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_COMMAND_QUEUE|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_COMMAND_ALLOCATOR)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_COMMAND_ALLOCATOR|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_PIPELINE_STATE)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_PIPELINE_STATE|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_COMMAND_LIST)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_COMMAND_LIST|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_FENCE)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_FENCE|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_DESCRIPTOR_HEAP)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_DESCRIPTOR_HEAP|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_HEAP)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_HEAP|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_QUERY_HEAP)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_QUERY_HEAP|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_COMMAND_SIGNATURE)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_COMMAND_SIGNATURE|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_PIPELINE_LIBRARY)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_PIPELINE_LIBRARY|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_VIDEO_DECODER)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_VIDEO_DECODER|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_VIDEO_PROCESSOR)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_VIDEO_PROCESSOR|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_RESOURCE)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_RESOURCE|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_PASS)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_PASS|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_CRYPTOSESSION)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_CRYPTOSESSION|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_CRYPTOSESSIONPOLICY)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_CRYPTOSESSIONPOLICY|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_PROTECTEDRESOURCESESSION)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_PROTECTEDRESOURCESESSION|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_VIDEO_DECODER_HEAP)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_VIDEO_DECODER_HEAP|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_COMMAND_POOL)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_COMMAND_POOL|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_COMMAND_RECORDER)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_COMMAND_RECORDER|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_STATE_OBJECT)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_STATE_OBJECT|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_METACOMMAND)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_METACOMMAND|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_SCHEDULINGGROUP)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_SCHEDULINGGROUP|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_VIDEO_MOTION_ESTIMATOR)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_VIDEO_MOTION_ESTIMATOR|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_VIDEO_MOTION_VECTOR_HEAP)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_VIDEO_MOTION_VECTOR_HEAP|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_VIDEO_EXTENSION_COMMAND)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_VIDEO_EXTENSION_COMMAND|";
			if (_Flags & D3D12_DRED_ALLOCATION_TYPE_INVALID)
				static_cast<_Mybase&>(*this) += "D3D12_DRED_ALLOCATION_TYPE_INVALID|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_DRED_ALLOCATION_TYPE)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_BACKGROUND_PROCESSING_MODE _Flags) {
			switch (_Flags) {
			case D3D12_BACKGROUND_PROCESSING_MODE_ALLOWED:
				static_cast<_Mybase&>(*this) = "D3D12_BACKGROUND_PROCESSING_MODE_ALLOWED";
				break;
			case D3D12_BACKGROUND_PROCESSING_MODE_ALLOW_INTRUSIVE_MEASUREMENTS:
				static_cast<_Mybase&>(*this) = "D3D12_BACKGROUND_PROCESSING_MODE_ALLOW_INTRUSIVE_MEASUREMENTS";
				break;
			case D3D12_BACKGROUND_PROCESSING_MODE_DISABLE_BACKGROUND_WORK:
				static_cast<_Mybase&>(*this) = "D3D12_BACKGROUND_PROCESSING_MODE_DISABLE_BACKGROUND_WORK";
				break;
			case D3D12_BACKGROUND_PROCESSING_MODE_DISABLE_PROFILING_BY_SYSTEM:
				static_cast<_Mybase&>(*this) = "D3D12_BACKGROUND_PROCESSING_MODE_DISABLE_PROFILING_BY_SYSTEM";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_BACKGROUND_PROCESSING_MODE)]");
			};
		}

		enum_string(D3D12_MEASUREMENTS_ACTION _Flags) {
			switch (_Flags) {
			case D3D12_MEASUREMENTS_ACTION_KEEP_ALL:
				static_cast<_Mybase&>(*this) = "D3D12_MEASUREMENTS_ACTION_KEEP_ALL";
				break;
			case D3D12_MEASUREMENTS_ACTION_COMMIT_RESULTS:
				static_cast<_Mybase&>(*this) = "D3D12_MEASUREMENTS_ACTION_COMMIT_RESULTS";
				break;
			case D3D12_MEASUREMENTS_ACTION_COMMIT_RESULTS_HIGH_PRIORITY:
				static_cast<_Mybase&>(*this) = "D3D12_MEASUREMENTS_ACTION_COMMIT_RESULTS_HIGH_PRIORITY";
				break;
			case D3D12_MEASUREMENTS_ACTION_DISCARD_PREVIOUS:
				static_cast<_Mybase&>(*this) = "D3D12_MEASUREMENTS_ACTION_DISCARD_PREVIOUS";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_MEASUREMENTS_ACTION)]");
			};
		}

		enum_string(D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD";
				break;
			case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE";
				break;
			case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR";
				break;
			case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE)]");
			};
		}

		enum_string(D3D12_RENDER_PASS_ENDING_ACCESS_TYPE _Flags) {
			switch (_Flags) {
			case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD";
				break;
			case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE";
				break;
			case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_RESOLVE:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_RESOLVE";
				break;
			case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS:
				static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RENDER_PASS_ENDING_ACCESS_TYPE)]");
			};
		}

		enum_string(D3D12_RENDER_PASS_FLAGS _Flags) {
			if (_Flags & D3D12_RENDER_PASS_FLAG_NONE)
				static_cast<_Mybase&>(*this) += "D3D12_RENDER_PASS_FLAG_NONE|";
			if (_Flags & D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES)
				static_cast<_Mybase&>(*this) += "D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES|";
			if (_Flags & D3D12_RENDER_PASS_FLAG_SUSPENDING_PASS)
				static_cast<_Mybase&>(*this) += "D3D12_RENDER_PASS_FLAG_SUSPENDING_PASS|";
			if (_Flags & D3D12_RENDER_PASS_FLAG_RESUMING_PASS)
				static_cast<_Mybase&>(*this) += "D3D12_RENDER_PASS_FLAG_RESUMING_PASS|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_RENDER_PASS_FLAG_NONE";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_RENDER_PASS_FLAGS)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_AXIS_SHADING_RATE _Flags) {
			if (_Flags & D3D12_AXIS_SHADING_RATE_1X)
				static_cast<_Mybase&>(*this) += "D3D12_AXIS_SHADING_RATE_1X|";
			if (_Flags & D3D12_AXIS_SHADING_RATE_2X)
				static_cast<_Mybase&>(*this) += "D3D12_AXIS_SHADING_RATE_2X|";
			if (_Flags & D3D12_AXIS_SHADING_RATE_4X)
				static_cast<_Mybase&>(*this) += "D3D12_AXIS_SHADING_RATE_4X|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_AXIS_SHADING_RATE_1X";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_AXIS_SHADING_RATE)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_SHADING_RATE _Flags) {
			if (_Flags & D3D12_SHADING_RATE_1X1)
				static_cast<_Mybase&>(*this) += "D3D12_SHADING_RATE_1X1|";
			if (_Flags & D3D12_SHADING_RATE_1X2)
				static_cast<_Mybase&>(*this) += "D3D12_SHADING_RATE_1X2|";
			if (_Flags & D3D12_SHADING_RATE_2X1)
				static_cast<_Mybase&>(*this) += "D3D12_SHADING_RATE_2X1|";
			if (_Flags & D3D12_SHADING_RATE_2X2)
				static_cast<_Mybase&>(*this) += "D3D12_SHADING_RATE_2X2|";
			if (_Flags & D3D12_SHADING_RATE_2X4)
				static_cast<_Mybase&>(*this) += "D3D12_SHADING_RATE_2X4|";
			if (_Flags & D3D12_SHADING_RATE_4X2)
				static_cast<_Mybase&>(*this) += "D3D12_SHADING_RATE_4X2|";
			if (_Flags & D3D12_SHADING_RATE_4X4)
				static_cast<_Mybase&>(*this) += "D3D12_SHADING_RATE_4X4|";
			if (_Mybase::empty()) {
				if (_Flags == 0) {
					static_cast<_Mybase&>(*this) = "D3D12_SHADING_RATE_1X1";
				}
				if (_Mybase::empty()) {
					throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_SHADING_RATE)]");
				}
			}
			else if (_Mybase::back() == '|') {
				_Mybase::pop_back();
			}
		}

		enum_string(D3D12_SHADING_RATE_COMBINER _Flags) {
			switch (_Flags) {
			case D3D12_SHADING_RATE_COMBINER_PASSTHROUGH:
				static_cast<_Mybase&>(*this) = "D3D12_SHADING_RATE_COMBINER_PASSTHROUGH";
				break;
			case D3D12_SHADING_RATE_COMBINER_OVERRIDE:
				static_cast<_Mybase&>(*this) = "D3D12_SHADING_RATE_COMBINER_OVERRIDE";
				break;
			case D3D12_SHADING_RATE_COMBINER_MIN:
				static_cast<_Mybase&>(*this) = "D3D12_SHADING_RATE_COMBINER_MIN";
				break;
			case D3D12_SHADING_RATE_COMBINER_MAX:
				static_cast<_Mybase&>(*this) = "D3D12_SHADING_RATE_COMBINER_MAX";
				break;
			case D3D12_SHADING_RATE_COMBINER_SUM:
				static_cast<_Mybase&>(*this) = "D3D12_SHADING_RATE_COMBINER_SUM";
				break;
			default:
				throw std::exception("[Unkown enum-item] ->[enum_string(D3D12_SHADING_RATE_COMBINER)]");
			};
		}
    };
}// namespace dx12

#endif