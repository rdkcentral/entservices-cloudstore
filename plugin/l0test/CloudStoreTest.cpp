/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../CloudStore.h"
#include "CloudStoreImplementationMock.h"
#include "ServiceMock.h"
#include <interfaces/json/JsonData_Store2.h>

using ::testing::_;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::Test;
using ::Thunder::Core::PublishedServiceType;
using ::Thunder::Exchange::IStore2;
using ::Thunder::JsonData::Store2::DeleteNamespaceParamsData;
using ::Thunder::JsonData::Store2::GetValueParamsInfo;
using ::Thunder::JsonData::Store2::GetValueResultData;
using ::Thunder::JsonData::Store2::SetValueParamsData;
using ::Thunder::Plugin::CloudStore;
using ::Thunder::PluginHost::IDispatcher;
using ::Thunder::PluginHost::IPlugin;

const auto kValue = "value_1";
const auto kKey = "key_1";
const auto kAppId = "app_id_1";
const auto kTtl = 100;

class ACloudStore : public Test {
protected:
    NiceMock<ServiceMock>* service;
    IPlugin* plugin;
    ACloudStore()
        : service(Thunder::Core::Service<NiceMock<ServiceMock>>::Create<NiceMock<ServiceMock>>())
        , plugin(Thunder::Core::Service<CloudStore>::Create<IPlugin>())
    {
    }
    ~ACloudStore() override
    {
        plugin->Release();
        service->Release();
    }
};

TEST_F(ACloudStore, GetsValueInAccountScopeViaJsonRpc)
{
    class CloudStoreImplementation : public NiceMock<CloudStoreImplementationMock> {
    public:
        CloudStoreImplementation()
        {
            EXPECT_CALL(*this, GetValue(_, _, _, _, _))
                .WillRepeatedly(Invoke(
                    [](const IStore2::ScopeType scope, const string& ns, const string& key, string& value, uint32_t& ttl) {
                        EXPECT_THAT(scope, Eq(IStore2::ScopeType::ACCOUNT));
                        EXPECT_THAT(ns, Eq(kAppId));
                        EXPECT_THAT(key, Eq(kKey));
                        value = kValue;
                        ttl = kTtl;
                        return Thunder::Core::ERROR_NONE;
                    }));
        }
    };
    PublishedServiceType<CloudStoreImplementation> metadata(Thunder::Core::System::MODULE_NAME, 1, 0, 0);
    ASSERT_THAT(plugin->Initialize(service), Eq(""));
    auto jsonRpc = plugin->QueryInterface<IDispatcher>();
    ASSERT_THAT(jsonRpc, NotNull());
    GetValueParamsInfo params;
    params.Scope = IStore2::ScopeType::ACCOUNT;
    params.Ns = kAppId;
    params.Key = kKey;
    string paramsJsonStr;
    params.ToString(paramsJsonStr);
    string resultJsonStr;
    ASSERT_THAT(jsonRpc->Invoke(0, 0, "", "getValue", paramsJsonStr, resultJsonStr), Eq(Thunder::Core::ERROR_NONE));
    GetValueResultData result;
    result.FromString(resultJsonStr);
    EXPECT_THAT(result.Value.Value(), Eq(kValue));
    EXPECT_THAT(result.Ttl.Value(), Eq(kTtl));
    jsonRpc->Release();
    plugin->Deinitialize(service);
}

TEST_F(ACloudStore, SetsValueInAccountScopeViaJsonRpc)
{
    class CloudStoreImplementation : public NiceMock<CloudStoreImplementationMock> {
    public:
        CloudStoreImplementation()
        {
            EXPECT_CALL(*this, SetValue(_, _, _, _, _))
                .WillRepeatedly(Invoke(
                    [](const IStore2::ScopeType scope, const string& ns, const string& key, const string& value, const uint32_t ttl) {
                        EXPECT_THAT(scope, Eq(IStore2::ScopeType::ACCOUNT));
                        EXPECT_THAT(ns, Eq(kAppId));
                        EXPECT_THAT(key, Eq(kKey));
                        EXPECT_THAT(value, Eq(kValue));
                        EXPECT_THAT(ttl, Eq(kTtl));
                        return Thunder::Core::ERROR_NONE;
                    }));
        }
    };
    PublishedServiceType<CloudStoreImplementation> metadata(Thunder::Core::System::MODULE_NAME, 1, 0, 0);
    ASSERT_THAT(plugin->Initialize(service), Eq(""));
    auto jsonRpc = plugin->QueryInterface<IDispatcher>();
    ASSERT_THAT(jsonRpc, NotNull());
    SetValueParamsData params;
    params.Scope = IStore2::ScopeType::ACCOUNT;
    params.Ns = kAppId;
    params.Key = kKey;
    params.Value = kValue;
    params.Ttl = kTtl;
    string paramsJsonStr;
    params.ToString(paramsJsonStr);
    string resultJsonStr;
    EXPECT_THAT(jsonRpc->Invoke(0, 0, "", "setValue", paramsJsonStr, resultJsonStr), Eq(Thunder::Core::ERROR_NONE));
    jsonRpc->Release();
    plugin->Deinitialize(service);
}

TEST_F(ACloudStore, DeletesKeyInAccountScopeViaJsonRpc)
{
    class CloudStoreImplementation : public NiceMock<CloudStoreImplementationMock> {
    public:
        CloudStoreImplementation()
        {
            EXPECT_CALL(*this, DeleteKey(_, _, _))
                .WillRepeatedly(Invoke(
                    [](const IStore2::ScopeType scope, const string& ns, const string& key) {
                        EXPECT_THAT(scope, Eq(IStore2::ScopeType::ACCOUNT));
                        EXPECT_THAT(ns, Eq(kAppId));
                        EXPECT_THAT(key, Eq(kKey));
                        return Thunder::Core::ERROR_NONE;
                    }));
        }
    };
    PublishedServiceType<CloudStoreImplementation> metadata(Thunder::Core::System::MODULE_NAME, 1, 0, 0);
    ASSERT_THAT(plugin->Initialize(service), Eq(""));
    auto jsonRpc = plugin->QueryInterface<IDispatcher>();
    ASSERT_THAT(jsonRpc, NotNull());
    GetValueParamsInfo params;
    params.Scope = IStore2::ScopeType::ACCOUNT;
    params.Ns = kAppId;
    params.Key = kKey;
    string paramsJsonStr;
    params.ToString(paramsJsonStr);
    string resultJsonStr;
    EXPECT_THAT(jsonRpc->Invoke(0, 0, "", "deleteKey", paramsJsonStr, resultJsonStr), Eq(Thunder::Core::ERROR_NONE));
    jsonRpc->Release();
    plugin->Deinitialize(service);
}

TEST_F(ACloudStore, DeletesNamespaceInAccountScopeViaJsonRpc)
{
    class CloudStoreImplementation : public NiceMock<CloudStoreImplementationMock> {
    public:
        CloudStoreImplementation()
        {
            EXPECT_CALL(*this, DeleteNamespace(_, _))
                .WillRepeatedly(Invoke(
                    [](const IStore2::ScopeType scope, const string& ns) {
                        EXPECT_THAT(scope, Eq(IStore2::ScopeType::ACCOUNT));
                        EXPECT_THAT(ns, Eq(kAppId));
                        return Thunder::Core::ERROR_NONE;
                    }));
        }
    };
    PublishedServiceType<CloudStoreImplementation> metadata(Thunder::Core::System::MODULE_NAME, 1, 0, 0);
    ASSERT_THAT(plugin->Initialize(service), Eq(""));
    auto jsonRpc = plugin->QueryInterface<IDispatcher>();
    ASSERT_THAT(jsonRpc, NotNull());
    DeleteNamespaceParamsData params;
    params.Scope = IStore2::ScopeType::ACCOUNT;
    params.Ns = kAppId;
    string paramsJsonStr;
    params.ToString(paramsJsonStr);
    string resultJsonStr;
    EXPECT_THAT(jsonRpc->Invoke(0, 0, "", "deleteNamespace", paramsJsonStr, resultJsonStr), Eq(Thunder::Core::ERROR_NONE));
    jsonRpc->Release();
    plugin->Deinitialize(service);
}
