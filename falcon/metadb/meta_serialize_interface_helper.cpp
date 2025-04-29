/* Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: MulanPSL-2.0
 */

extern "C" {
#include "metadb/meta_serialize_interface_helper.h"
}

#include "falcon_meta_param_generated.h"
#include "falcon_meta_response_generated.h"
#include "falcon_meta_rpc.pb.h"

static flatbuffers::FlatBufferBuilder FlatBufferBuilderPerProcess;

FalconSupportMetaService MetaServiceTypeDecode(int32_t type)
{
    switch (type) {
    case falcon::meta_proto::MetaServiceType::MKDIR:
        return FalconSupportMetaService::MKDIR;
    case falcon::meta_proto::MetaServiceType::MKDIR_SUB_MKDIR:
        return FalconSupportMetaService::MKDIR_SUB_MKDIR;
    case falcon::meta_proto::MetaServiceType::MKDIR_SUB_CREATE:
        return FalconSupportMetaService::MKDIR_SUB_CREATE;
    case falcon::meta_proto::MetaServiceType::CREATE:
        return FalconSupportMetaService::CREATE;
    case falcon::meta_proto::MetaServiceType::STAT:
        return FalconSupportMetaService::STAT;
    case falcon::meta_proto::MetaServiceType::OPEN:
        return FalconSupportMetaService::OPEN;
    case falcon::meta_proto::MetaServiceType::CLOSE:
        return FalconSupportMetaService::CLOSE;
    case falcon::meta_proto::MetaServiceType::UNLINK:
        return FalconSupportMetaService::UNLINK;
    case falcon::meta_proto::MetaServiceType::READDIR:
        return FalconSupportMetaService::READDIR;
    case falcon::meta_proto::MetaServiceType::OPENDIR:
        return FalconSupportMetaService::OPENDIR;
    case falcon::meta_proto::MetaServiceType::RMDIR:
        return FalconSupportMetaService::RMDIR;
    case falcon::meta_proto::MetaServiceType::RMDIR_SUB_RMDIR:
        return FalconSupportMetaService::RMDIR_SUB_RMDIR;
    case falcon::meta_proto::MetaServiceType::RMDIR_SUB_UNLINK:
        return FalconSupportMetaService::RMDIR_SUB_UNLINK;
    case falcon::meta_proto::MetaServiceType::RENAME:
        return FalconSupportMetaService::RENAME;
    case falcon::meta_proto::MetaServiceType::RENAME_SUB_RENAME_LOCALLY:
        return FalconSupportMetaService::RENAME_SUB_RENAME_LOCALLY;
    case falcon::meta_proto::MetaServiceType::RENAME_SUB_CREATE:
        return FalconSupportMetaService::RENAME_SUB_CREATE;
    case falcon::meta_proto::MetaServiceType::UTIMENS:
        return FalconSupportMetaService::UTIMENS;
    case falcon::meta_proto::MetaServiceType::CHOWN:
        return FalconSupportMetaService::CHOWN;
    case falcon::meta_proto::MetaServiceType::CHMOD:
        return FalconSupportMetaService::CHMOD;
    default:
        return FalconSupportMetaService::NOT_SUPPORTED;
    }
}

int32_t MetaServiceTypeEncode(FalconSupportMetaService metaService)
{
    switch (metaService) {
    case FalconSupportMetaService::MKDIR:
        return falcon::meta_proto::MetaServiceType::MKDIR;
    case FalconSupportMetaService::MKDIR_SUB_MKDIR:
        return falcon::meta_proto::MetaServiceType::MKDIR_SUB_MKDIR;
    case FalconSupportMetaService::MKDIR_SUB_CREATE:
        return falcon::meta_proto::MetaServiceType::MKDIR_SUB_CREATE;
    case FalconSupportMetaService::CREATE:
        return falcon::meta_proto::MetaServiceType::CREATE;
    case FalconSupportMetaService::STAT:
        return falcon::meta_proto::MetaServiceType::STAT;
    case FalconSupportMetaService::OPEN:
        return falcon::meta_proto::MetaServiceType::OPEN;
    case FalconSupportMetaService::CLOSE:
        return falcon::meta_proto::MetaServiceType::CLOSE;
    case FalconSupportMetaService::UNLINK:
        return falcon::meta_proto::MetaServiceType::UNLINK;
    case FalconSupportMetaService::READDIR:
        return falcon::meta_proto::MetaServiceType::READDIR;
    case FalconSupportMetaService::OPENDIR:
        return falcon::meta_proto::MetaServiceType::OPENDIR;
    case FalconSupportMetaService::RMDIR:
        return falcon::meta_proto::MetaServiceType::RMDIR;
    case FalconSupportMetaService::RMDIR_SUB_RMDIR:
        return falcon::meta_proto::MetaServiceType::RMDIR_SUB_RMDIR;
    case FalconSupportMetaService::RMDIR_SUB_UNLINK:
        return falcon::meta_proto::MetaServiceType::RMDIR_SUB_UNLINK;
    case FalconSupportMetaService::RENAME:
        return falcon::meta_proto::MetaServiceType::RENAME;
    case FalconSupportMetaService::RENAME_SUB_RENAME_LOCALLY:
        return falcon::meta_proto::MetaServiceType::RENAME_SUB_RENAME_LOCALLY;
    case FalconSupportMetaService::RENAME_SUB_CREATE:
        return falcon::meta_proto::MetaServiceType::RENAME_SUB_CREATE;
    case FalconSupportMetaService::UTIMENS:
        return falcon::meta_proto::MetaServiceType::UTIMENS;
    case FalconSupportMetaService::CHOWN:
        return falcon::meta_proto::MetaServiceType::CHOWN;
    case FalconSupportMetaService::CHMOD:
        return falcon::meta_proto::MetaServiceType::CHMOD;
    default:
        return -1;
    }
}

bool SerializedDataMetaParamDecode(FalconSupportMetaService metaService,
                                   int count,
                                   SerializedData *param,
                                   MetaProcessInfoData *infoArray)
{
    sd_size_t p = 0;
    for (int i = 0; i < count; ++i) {
        uint8_t *buffer = (uint8_t *)param->buffer + p;
        sd_size_t size = SerializedDataNextSeveralItemSize(param, p, 1);
        if (size == (sd_size_t)-1) {
            printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
            return false;
        }

        uint8_t *itemBuffer = (uint8_t *)buffer + SERIALIZED_DATA_ALIGNMENT;
        size_t itemSize = size - SERIALIZED_DATA_ALIGNMENT;
        flatbuffers::Verifier verifier(itemBuffer, itemSize);
        if (!verifier.VerifyBuffer<falcon::meta_fbs::MetaParam>(NULL)) {
            printf("[debug] itemSize = %lu, serialized param is corrupt: %s:%d\n", itemSize, __FILE__, __LINE__);
            return false;
        }
        auto metaParam = falcon::meta_fbs::GetMetaParam(itemBuffer);

        MetaProcessInfo info = infoArray + i;
        switch (metaService) {
        case FalconSupportMetaService::MKDIR:
        case FalconSupportMetaService::CREATE:
        case FalconSupportMetaService::STAT:
        case FalconSupportMetaService::OPEN:
        case FalconSupportMetaService::UNLINK:
        case FalconSupportMetaService::OPENDIR:
        case FalconSupportMetaService::RMDIR: {
            // path only param
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_PathOnlyParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            info->path = metaParam->param_as_PathOnlyParam()->path()->c_str();
            break;
        }
        case FalconSupportMetaService::MKDIR_SUB_MKDIR: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_MkdirSubMkdirParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto mkdirSubMkdirParam = metaParam->param_as_MkdirSubMkdirParam();
            info->parentId = mkdirSubMkdirParam->parent_id();
            info->name = const_cast<char *>(mkdirSubMkdirParam->name()->c_str());
            info->inodeId = mkdirSubMkdirParam->inode_id();
            break;
        }
        case FalconSupportMetaService::MKDIR_SUB_CREATE: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_MkdirSubCreateParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto mkdirSubCreateParam = metaParam->param_as_MkdirSubCreateParam();
            info->parentId_partId = mkdirSubCreateParam->parent_id_part_id();
            info->name = const_cast<char *>(mkdirSubCreateParam->name()->c_str());
            info->inodeId = mkdirSubCreateParam->inode_id();
            info->st_mode = mkdirSubCreateParam->st_mode();
            info->st_size = mkdirSubCreateParam->st_size();
            info->st_mtim = mkdirSubCreateParam->st_mtim();
            break;
        }
        case FalconSupportMetaService::CLOSE: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_CloseParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto closeParam = metaParam->param_as_CloseParam();
            info->path = closeParam->path()->c_str();
            info->st_size = closeParam->st_size();
            info->st_mtim = closeParam->st_mtim();
            info->node_id = closeParam->node_id();
            break;
        }
        case FalconSupportMetaService::READDIR: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_ReadDirParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto readDirParam = metaParam->param_as_ReadDirParam();
            info->path = readDirParam->path()->c_str();
            info->readDirMaxReadCount = readDirParam->max_read_count();
            info->readDirLastShardIndex = readDirParam->last_shard_index();
            info->readDirLastFileName = readDirParam->last_file_name()->c_str();
            break;
        }
        case FalconSupportMetaService::RMDIR_SUB_RMDIR: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_RmdirSubRmdirParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto rmdirSubRmdirParam = metaParam->param_as_RmdirSubRmdirParam();
            info->parentId = rmdirSubRmdirParam->parent_id();
            info->name = const_cast<char *>(rmdirSubRmdirParam->name()->c_str());
            break;
        }
        case FalconSupportMetaService::RMDIR_SUB_UNLINK: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_RmdirSubUnlinkParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto rmdirSubUnlinkParam = metaParam->param_as_RmdirSubUnlinkParam();
            info->parentId_partId = rmdirSubUnlinkParam->parent_id_part_id();
            info->name = const_cast<char *>(rmdirSubUnlinkParam->name()->c_str());
            break;
        }
        case FalconSupportMetaService::RENAME: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_RenameParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto renameParam = metaParam->param_as_RenameParam();
            info->path = renameParam->src()->c_str();
            info->dstPath = renameParam->dst()->c_str();
            break;
        }
        case FalconSupportMetaService::RENAME_SUB_RENAME_LOCALLY: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_RenameSubRenameLocallyParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto renameSubRenameLocallyParam = metaParam->param_as_RenameSubRenameLocallyParam();
            info->parentId = renameSubRenameLocallyParam->src_parent_id();
            info->parentId_partId = renameSubRenameLocallyParam->src_parent_id_part_id();
            info->name = const_cast<char *>(renameSubRenameLocallyParam->src_name()->c_str());
            info->dstParentId =
                renameSubRenameLocallyParam->dst_parent_id(); // TOFO: ymz: modify 'dstParentIdPartId' to 'dstParentId'
            info->dstParentIdPartId = renameSubRenameLocallyParam->dst_parent_id_part_id();
            info->dstName = const_cast<char *>(renameSubRenameLocallyParam->dst_name()->c_str());
            info->targetIsDirectory = renameSubRenameLocallyParam->target_is_directory();
            info->srcLockOrder = renameSubRenameLocallyParam->src_lock_order();
            info->inodeId = renameSubRenameLocallyParam->directory_inode_id();
            break;
        }
        case FalconSupportMetaService::RENAME_SUB_CREATE: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_RenameSubCreateParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto renameSubCreateParam = metaParam->param_as_RenameSubCreateParam();
            info->parentId_partId = renameSubCreateParam->parentid_partid();
            info->name = const_cast<char *>(renameSubCreateParam->name()->c_str());
            info->inodeId = renameSubCreateParam->st_ino();
            info->st_dev = renameSubCreateParam->st_dev();
            info->st_mode = renameSubCreateParam->st_mode();
            info->st_nlink = renameSubCreateParam->st_nlink();
            info->st_uid = renameSubCreateParam->st_uid();
            info->st_gid = renameSubCreateParam->st_gid();
            info->st_rdev = renameSubCreateParam->st_rdev();
            info->st_size = renameSubCreateParam->st_size();
            info->st_blksize = renameSubCreateParam->st_blksize();
            info->st_blocks = renameSubCreateParam->st_blocks();
            info->st_atim = renameSubCreateParam->st_atim();
            info->st_mtim = renameSubCreateParam->st_mtim();
            info->st_ctim = renameSubCreateParam->st_ctim();
            info->node_id = renameSubCreateParam->node_id();
            break;
        }
        case FalconSupportMetaService::UTIMENS: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_UtimeNsParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto utimeNsParam = metaParam->param_as_UtimeNsParam();
            info->path = utimeNsParam->path()->c_str();
            info->st_atim = utimeNsParam->st_atim();
            info->st_mtim = utimeNsParam->st_mtim();
            break;
        }
        case FalconSupportMetaService::CHOWN: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_ChownParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto chownParam = metaParam->param_as_ChownParam();
            info->path = chownParam->path()->c_str();
            info->st_uid = chownParam->st_uid();
            info->st_gid = chownParam->st_gid();
            break;
        }
        case FalconSupportMetaService::CHMOD: {
            if (metaParam->param_type() != falcon::meta_fbs::AnyMetaParam::AnyMetaParam_ChmodParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto chmodParam = metaParam->param_as_ChmodParam();
            info->path = chmodParam->path()->c_str();
            info->st_mode = chmodParam->st_mode();
            break;
        }
        default:
            printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
            return false;
        }

        p += size;
    }
    return true;
}

bool SerializedDataMetaParamEncode(FalconSupportMetaService metaService,
                                   MetaProcessInfo *infoArray,
                                   int32_t *index,
                                   int count,
                                   flatbuffers::FlatBufferBuilder &builder,
                                   SerializedData *param)
{
    for (int i = 0; i < count; ++i) {
        MetaProcessInfo info = index == NULL ? (infoArray[i]) : (infoArray[index[i]]);

        builder.Clear();
        flatbuffers::Offset<falcon::meta_fbs::MetaParam> metaParam;
        switch (metaService) {
        case FalconSupportMetaService::MKDIR_SUB_MKDIR: {
            auto mkdirSubMkdirParam =
                falcon::meta_fbs::CreateMkdirSubMkdirParamDirect(builder, info->parentId, info->name, info->inodeId);
            metaParam = falcon::meta_fbs::CreateMetaParam(builder,
                                                          falcon::meta_fbs::AnyMetaParam_MkdirSubMkdirParam,
                                                          mkdirSubMkdirParam.Union());
            break;
        }
        case FalconSupportMetaService::MKDIR_SUB_CREATE: {
            auto mkdirSubCreateParam = falcon::meta_fbs::CreateMkdirSubCreateParamDirect(builder,
                                                                                         info->parentId_partId,
                                                                                         info->name,
                                                                                         info->inodeId,
                                                                                         info->st_mode,
                                                                                         info->st_mtim,
                                                                                         info->st_size);
            metaParam = falcon::meta_fbs::CreateMetaParam(builder,
                                                          falcon::meta_fbs::AnyMetaParam_MkdirSubCreateParam,
                                                          mkdirSubCreateParam.Union());
            break;
        }
        case FalconSupportMetaService::RMDIR_SUB_RMDIR: {
            auto rmdirSubRmdirParam =
                falcon::meta_fbs::CreateRmdirSubRmdirParamDirect(builder, info->parentId, info->name);
            metaParam = falcon::meta_fbs::CreateMetaParam(builder,
                                                          falcon::meta_fbs::AnyMetaParam_RmdirSubRmdirParam,
                                                          rmdirSubRmdirParam.Union());
            break;
        }
        case FalconSupportMetaService::RMDIR_SUB_UNLINK: {
            auto rmdirSubUnlinkParam =
                falcon::meta_fbs::CreateRmdirSubUnlinkParamDirect(builder, info->parentId_partId, info->name);
            metaParam = falcon::meta_fbs::CreateMetaParam(builder,
                                                          falcon::meta_fbs::AnyMetaParam_RmdirSubUnlinkParam,
                                                          rmdirSubUnlinkParam.Union());
            break;
        }
        case FalconSupportMetaService::RENAME_SUB_RENAME_LOCALLY: {
            auto renameSubRenameLocallyParam =
                falcon::meta_fbs::CreateRenameSubRenameLocallyParamDirect(builder,
                                                                          info->parentId,
                                                                          info->parentId_partId,
                                                                          info->name,
                                                                          info->dstParentId,
                                                                          info->dstParentIdPartId,
                                                                          info->dstName,
                                                                          info->targetIsDirectory,
                                                                          info->inodeId,
                                                                          info->srcLockOrder);
            metaParam = falcon::meta_fbs::CreateMetaParam(builder,
                                                          falcon::meta_fbs::AnyMetaParam_RenameSubRenameLocallyParam,
                                                          renameSubRenameLocallyParam.Union());
            break;
        }
        case FalconSupportMetaService::RENAME_SUB_CREATE: {
            auto renameSubCreateParam = falcon::meta_fbs::CreateRenameSubCreateParamDirect(builder,
                                                                                           info->parentId_partId,
                                                                                           info->name,
                                                                                           info->inodeId,
                                                                                           info->st_dev,
                                                                                           info->st_mode,
                                                                                           info->st_nlink,
                                                                                           info->st_uid,
                                                                                           info->st_gid,
                                                                                           info->st_rdev,
                                                                                           info->st_size,
                                                                                           info->st_blksize,
                                                                                           info->st_blocks,
                                                                                           info->st_atim,
                                                                                           info->st_mtim,
                                                                                           info->st_ctim,
                                                                                           info->node_id);
            metaParam = falcon::meta_fbs::CreateMetaParam(builder,
                                                          falcon::meta_fbs::AnyMetaParam_RenameSubCreateParam,
                                                          renameSubCreateParam.Union());
            break;
        }
        default:
            return false;
        }
        builder.Finish(metaParam);

        char *buffer = SerializedDataApplyForSegment(param, builder.GetSize());
        memcpy(buffer, builder.GetBufferPointer(), builder.GetSize());
    }
    return true;
}

bool SerializedDataMetaParamEncodeWithPerProcessFlatBufferBuilder(FalconSupportMetaService metaService,
                                                                  MetaProcessInfo *infoArray,
                                                                  int32_t *index,
                                                                  int count,
                                                                  SerializedData *param)
{
    return SerializedDataMetaParamEncode(metaService, infoArray, index, count, FlatBufferBuilderPerProcess, param);
}

bool SerializedDataMetaResponseDecode(FalconSupportMetaService metaService,
                                      int count,
                                      SerializedData *response,
                                      MetaProcessInfoData *infoArray)
{
    sd_size_t p = 0;
    for (int i = 0; i < count; i++) {
        uint8_t *buffer = (uint8_t *)response->buffer + p;
        sd_size_t size = SerializedDataNextSeveralItemSize(response, p, 1);
        if (size == (sd_size_t)-1)
            return false;

        uint8_t *itemBuffer = (uint8_t *)buffer + SERIALIZED_DATA_ALIGNMENT;
        size_t itemSize = size - SERIALIZED_DATA_ALIGNMENT;
        flatbuffers::Verifier verifier(itemBuffer, itemSize);
        if (!verifier.VerifyBuffer<falcon::meta_fbs::MetaResponse>(NULL))
            return false;
        auto metaResponse = falcon::meta_fbs::GetMetaResponse(itemBuffer);

        MetaProcessInfo info = infoArray + i;
        info->errorCode = (FalconErrorCode)metaResponse->error_code();
        switch (metaService) {
        case FalconSupportMetaService::MKDIR_SUB_MKDIR:
        case FalconSupportMetaService::MKDIR_SUB_CREATE:
        case FalconSupportMetaService::RMDIR_SUB_RMDIR:
        case FalconSupportMetaService::RMDIR_SUB_UNLINK:
        case FalconSupportMetaService::RENAME_SUB_CREATE: {
            // Error code only. Do nothing.
            break;
        }
        case FalconSupportMetaService::RENAME_SUB_RENAME_LOCALLY: {
            if (metaResponse->response_type() == falcon::meta_fbs::AnyMetaResponse::AnyMetaResponse_NONE) {
                // No extra data returned. Do nothing.
            } else if (metaResponse->response_type() ==
                       falcon::meta_fbs::AnyMetaResponse::AnyMetaResponse_RenameSubRenameLocallyResponse) {
                //
                auto renameSubRenameLocallyResponse = metaResponse->response_as_RenameSubRenameLocallyResponse();
                info->inodeId = renameSubRenameLocallyResponse->st_ino();
                info->st_dev = renameSubRenameLocallyResponse->st_dev();
                info->st_mode = renameSubRenameLocallyResponse->st_mode();
                info->st_nlink = renameSubRenameLocallyResponse->st_nlink();
                info->st_uid = renameSubRenameLocallyResponse->st_uid();
                info->st_gid = renameSubRenameLocallyResponse->st_gid();
                info->st_rdev = renameSubRenameLocallyResponse->st_rdev();
                info->st_size = renameSubRenameLocallyResponse->st_size();
                info->st_blksize = renameSubRenameLocallyResponse->st_blksize();
                info->st_blocks = renameSubRenameLocallyResponse->st_blocks();
                info->st_atim = renameSubRenameLocallyResponse->st_atim();
                info->st_mtim = renameSubRenameLocallyResponse->st_mtim();
                info->st_ctim = renameSubRenameLocallyResponse->st_ctim();
                info->node_id = renameSubRenameLocallyResponse->node_id();
            } else
                return false;
            break;
        }
        default:
            return false;
        }

        p += size;
    }
    return true;
}

static bool SerializedDataMetaResponseEncode(FalconSupportMetaService metaService,
                                             int count,
                                             MetaProcessInfoData *infoArray,
                                             flatbuffers::FlatBufferBuilder &builder,
                                             SerializedData *response)
{
    for (int i = 0; i < count; ++i) {
        builder.Clear();
        MetaProcessInfo info = infoArray + i;
        flatbuffers::Offset<falcon::meta_fbs::MetaResponse> metaResponse;
        if (info->errorCode != SUCCESS && info->errorCode != FILE_EXISTS) {
            //
            metaResponse = falcon::meta_fbs::CreateMetaResponse(builder, info->errorCode);
        } else {
            switch (metaService) {
            case FalconSupportMetaService::MKDIR:
            case FalconSupportMetaService::MKDIR_SUB_MKDIR:
            case FalconSupportMetaService::MKDIR_SUB_CREATE:
            case FalconSupportMetaService::CLOSE:
            case FalconSupportMetaService::RMDIR:
            case FalconSupportMetaService::RMDIR_SUB_RMDIR:
            case FalconSupportMetaService::RMDIR_SUB_UNLINK:
            case FalconSupportMetaService::RENAME:
            case FalconSupportMetaService::RENAME_SUB_CREATE:
            case FalconSupportMetaService::UTIMENS:
            case FalconSupportMetaService::CHOWN:
            case FalconSupportMetaService::CHMOD: {
                // error code only response
                metaResponse = falcon::meta_fbs::CreateMetaResponse(builder, info->errorCode);
                break;
            }
            case FalconSupportMetaService::CREATE: {
                // auto createResponse = falcon::meta_fbs::CreateCreateResponse(builder, info->inodeId);
                auto createResponse = falcon::meta_fbs::CreateCreateResponse(builder,
                                                                             info->inodeId,
                                                                             info->node_id,
                                                                             info->st_dev,
                                                                             info->st_mode,
                                                                             info->st_nlink,
                                                                             info->st_uid,
                                                                             info->st_gid,
                                                                             info->st_rdev,
                                                                             info->st_size,
                                                                             info->st_blksize,
                                                                             info->st_blocks,
                                                                             info->st_atim,
                                                                             info->st_mtim,
                                                                             info->st_ctim);
                metaResponse = falcon::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    falcon::meta_fbs::AnyMetaResponse_CreateResponse,
                                                                    createResponse.Union());
                break;
            }
            case FalconSupportMetaService::STAT: {
                auto statResponse = falcon::meta_fbs::CreateStatResponse(builder,
                                                                         info->inodeId,
                                                                         info->st_dev,
                                                                         info->st_mode,
                                                                         info->st_nlink,
                                                                         info->st_uid,
                                                                         info->st_gid,
                                                                         info->st_rdev,
                                                                         info->st_size,
                                                                         info->st_blksize,
                                                                         info->st_blocks,
                                                                         info->st_atim,
                                                                         info->st_mtim,
                                                                         info->st_ctim);
                metaResponse = falcon::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    falcon::meta_fbs::AnyMetaResponse_StatResponse,
                                                                    statResponse.Union());
                break;
            }
            case FalconSupportMetaService::OPEN: {
                auto openResponse = falcon::meta_fbs::CreateOpenResponse(builder,
                                                                         info->inodeId,
                                                                         info->node_id,
                                                                         info->st_dev,
                                                                         info->st_mode,
                                                                         info->st_nlink,
                                                                         info->st_uid,
                                                                         info->st_gid,
                                                                         info->st_rdev,
                                                                         info->st_size,
                                                                         info->st_blksize,
                                                                         info->st_blocks,
                                                                         info->st_atim,
                                                                         info->st_mtim,
                                                                         info->st_ctim);
                metaResponse = falcon::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    falcon::meta_fbs::AnyMetaResponse_OpenResponse,
                                                                    openResponse.Union());
                break;
            }
            case FalconSupportMetaService::UNLINK: {
                auto unlinkResponse =
                    falcon::meta_fbs::CreateUnlinkResponse(builder, info->inodeId, info->st_size, info->node_id);
                metaResponse = falcon::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    falcon::meta_fbs::AnyMetaResponse_UnlinkResponse,
                                                                    unlinkResponse.Union());
                break;
            }
            case FalconSupportMetaService::READDIR: {
                std::vector<flatbuffers::Offset<falcon::meta_fbs::OneReadDirResponse>> readDirResultList;
                for (int j = 0; j < info->readDirResultCount; ++j)
                    readDirResultList.push_back(
                        falcon::meta_fbs::CreateOneReadDirResponseDirect(builder,
                                                                         info->readDirResultList[j]->fileName,
                                                                         info->readDirResultList[j]->mode));
                auto readDirResponse = falcon::meta_fbs::CreateReadDirResponseDirect(builder,
                                                                                     info->readDirLastShardIndex,
                                                                                     info->readDirLastFileName,
                                                                                     &readDirResultList);
                metaResponse = falcon::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    falcon::meta_fbs::AnyMetaResponse_ReadDirResponse,
                                                                    readDirResponse.Union());
                break;
            }
            case FalconSupportMetaService::OPENDIR: {
                auto openDirResponse = falcon::meta_fbs::CreateOpenDirResponse(builder, info->inodeId);
                metaResponse = falcon::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    falcon::meta_fbs::AnyMetaResponse_OpenDirResponse,
                                                                    openDirResponse.Union());
                break;
            }
            case FalconSupportMetaService::RENAME_SUB_RENAME_LOCALLY: {
                if (info->parentId_partId != 0 && info->dstParentIdPartId == 0) {
                    auto renameSubRenameLocallyResponse =
                        falcon::meta_fbs::CreateRenameSubRenameLocallyResponse(builder,
                                                                               info->inodeId,
                                                                               info->st_dev,
                                                                               info->st_mode,
                                                                               info->st_nlink,
                                                                               info->st_uid,
                                                                               info->st_gid,
                                                                               info->st_rdev,
                                                                               info->st_size,
                                                                               info->st_blksize,
                                                                               info->st_blocks,
                                                                               info->st_atim,
                                                                               info->st_mtim,
                                                                               info->st_ctim,
                                                                               info->node_id);
                    metaResponse = falcon::meta_fbs::CreateMetaResponse(
                        builder,
                        info->errorCode,
                        falcon::meta_fbs::AnyMetaResponse_RenameSubRenameLocallyResponse,
                        renameSubRenameLocallyResponse.Union());
                } else {
                    metaResponse = falcon::meta_fbs::CreateMetaResponse(builder, info->errorCode);
                }
                break;
            }
            default:
                return false;
            }
        }
        builder.Finish(metaResponse);

        char *buffer = SerializedDataApplyForSegment(response, builder.GetSize());
        memcpy(buffer, builder.GetBufferPointer(), builder.GetSize());
    }
    return true;
}

bool SerializedDataMetaResponseEncodeWithPerProcessFlatBufferBuilder(FalconSupportMetaService metaService,
                                                                     int count,
                                                                     MetaProcessInfoData *infoArray,
                                                                     SerializedData *response)
{
    return SerializedDataMetaResponseEncode(metaService, count, infoArray, FlatBufferBuilderPerProcess, response);
}
