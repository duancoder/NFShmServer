﻿// -------------------------------------------------------------------------
//    @FileName         :    NFNamingHandle.h
//    @Author           :    xxxxx
//    @Date             :   xxxx-xx-xx
//    @Email			:    xxxxxxxxx@xxx.xxx
//    @Module           :    NFNamingHandle.h
//
// -------------------------------------------------------------------------

#pragma once

#include <list>
#include <set>
#include <string>

#include "NFZookeeperClient.h"
#include "NFZookeeperNaming.h"

struct NameInfo
{
    NameInfo() : _version(0) {}

    int64_t _version;
    std::vector<std::string> _urls;
};

typedef std::pair<std::string, NameInfo> GetNameResult;
typedef std::map<std::string, NameInfo>  GetAllNameResults;

/// @brief 异步查询接口的执行结果回调
/// @param rc 执行结果返回值
typedef std::function<void(int rc, GetNameResult& urls)> CbGetReturn;

/// @brief 异步查询接口的执行结果回调
/// @param rc 执行结果返回值
typedef std::function<void(int rc, GetAllNameResults& all_urls)> CbGetAllReturn;

typedef std::map<std::string, std::string> CreatedInstsNameMap;

class NamingHandle {
public:
    explicit NamingHandle(NFZookeeperClient* zk_client)
            :   m_zk_client(zk_client), m_insts_map(NULL), m_retry_times(3) {
    }
    NamingHandle(NFZookeeperClient* zk_client, CreatedInstsNameMap* insts)
            :   m_zk_client(zk_client), m_insts_map(insts), m_retry_times(3) {
    }

    virtual ~NamingHandle() {}

    int32_t ValueVec2String(const std::vector<std::string>& urls, std::string* out_str);

    int32_t String2ValueVec(const std::string& in_str, std::vector<std::string>* urls);

    int32_t MakeInstNodeNameAndVal(const std::vector<std::string>& urls,
                                   std::string* inst_name,
                                   std::string* inst_val);

protected:
    NFZookeeperClient*        m_zk_client;
    CreatedInstsNameMap*    m_insts_map;
    int32_t                 m_retry_times;
};

class CreateInstHandle : public NamingHandle {
public:
    CreateInstHandle(NFZookeeperClient* zk_client, CreatedInstsNameMap* insts);

    virtual ~CreateInstHandle() {}

    int32_t Start(const std::string& name,
                  const std::vector<std::string>& urls,
                  const std::string& digestpwd,
                  NFNamingCbReturnCode cb);

    int32_t Exit(int32_t rc);

    // 1-1.检查path是否存在
    void CheckNamingNode();

    void CbCheckNamingNode(int rc, const char *value, int value_len, const Stat *stat);

    // 1-2.递归的创建名字节点
    void CreateNamingNode(uint64_t offset);

    void CbCreateNamingNode(uint64_t offset, int rc, const char *value);

    // 2.创建instance节点
    void CreateInstNode();

    void CbCreateInstNode(int rc, const char *value);

private:
    std::string         m_name;
    std::string         m_inst_name;
    std::string         m_inst_node;
    std::string         m_inst_value;
    std::string         m_digestpwd;
    NFNamingCbReturnCode        m_return_callback;
};

class DelInstHandle : public NamingHandle {
public:
    DelInstHandle(NFZookeeperClient* zk_client, CreatedInstsNameMap* insts);

    virtual ~DelInstHandle() {}

    int32_t Start(const std::string& name,
                  NFNamingCbReturnCode cb);

    int32_t ForceStart(const std::string& name, const std::vector<std::string>& urls,
                       NFNamingCbReturnCode cb);

    int32_t Exit(int32_t rc);

    // 1.删除子节点
    int32_t DeleteInstNode();

    // 1.删除子节点
    int32_t ForceDeleteInstNode();

    void CbDeleteInstNode(int rc);
    void CbForceDeleteInstNode(int rc);

    // 2.尝试删除名字节点
    void DeleteNamingNode(uint64_t offset);

    void CbDeleteNamingNode(uint64_t offset, int rc);

private:
    std::string     m_name;
    std::string     m_inst_node_name;
    std::string     m_inst_name;
    std::string     m_inst_value;
    NFNamingCbReturnCode    m_cb;
};

class GetValueHandle : public NamingHandle {
public:
    explicit GetValueHandle(NFZookeeperClient* zk_client);

    virtual ~GetValueHandle() {}

    int32_t Start(const std::string& name,
                  CbGetReturn cb,
                  bool is_watch);

    int32_t Exit(int32_t rc);

    void GetPathChild();

    void CbGetPathChild();

    // 1.获取inst子节点
    void GetInst();

    void CbGetInst(int rc, const String_vector *strings, const Stat *stat);

private:
    // 标记已经回调了，对任何一个inst访问失败都失败，导致失败可能提前回调
    bool                m_has_cb;
    int32_t             m_is_watch;
    int32_t             m_wait_num;

    GetNameResult       m_get_name_result;
    CbGetReturn         m_return_callback;
};

class GetExpandedValueHandle : public NamingHandle {
public:
    explicit GetExpandedValueHandle(NFZookeeperClient* zk_client);

    virtual ~GetExpandedValueHandle() {}

    int32_t Start(const std::string& name,
                  CbGetAllReturn cb,
                  bool is_watch);

    int32_t Exit(int32_t rc);

    // 1.展开service_path
    void ExpandedPath();

    void CbExpandedPath(std::string name, int rc,
                        const String_vector *strings, const Stat *stat);

    // 2.分别查询各个名字的值
    void GetNamingValue(const std::string& name);

    void CbGetNamingValue(int rc, GetNameResult& get_name_return); // NOLINT

private:
    int32_t SplitPathWildcard();

    // 标记已经回调了，对任何一个service name访问失败都失败，导致失败可能提前回调
    bool                m_has_cb;
    int32_t             m_is_watch;
    int32_t             m_wait_num;

    std::list<std::string>  m_all_paths;
    std::string             m_name;
    std::string             m_path_prefix;
    std::string             m_path_subfix;
    std::string             m_path_remain;

    GetAllNameResults     m_get_all_name_result;
    CbGetAllReturn       m_return_callback;
};

