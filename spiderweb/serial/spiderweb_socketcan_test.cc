#include "spiderweb/serial/spiderweb_socketcan.h"

#include <spiderweb/io/spiderweb_binary_writer.hpp>

#include "spiderweb/ppk_assert.h"

// enum class CommandType {
//   kYkSelect = 0x04,           // 遥控选择
//   kYkExec = 0x05,             // 遥控执行
//   kYkCancel = 0x06,           // 遥控撤消
//   kUploadValue = 0x07,        // 传定值
//   kWriteValue = 0x08,         // 写定值
//   kReadValue = 0x09,          // 读定值
//   kReset = 0x11,              // 信号复归
//   kReadVersion = 0x80,        // 读版本信息
//   kReadCom = 0x81,            // 读通讯配置
//   kWriteCom = 0x82,           // 写通讯配置
//   kSOE = 0x22,                // SOE上送
//   kYkSelectReturn = 0x24,     // 遥控选择返回
//   kYkExecReturn = 0x25,       // 遥控执行返回
//   kYkCancelReturn = 0x26,     // 遥控撤消返回
//   kUploadValueReturn = 0x27,  // 传定值返回
//   kWriteValueReturn = 0x28,   // 写定值返回
//   kReadValueReturn = 0x29,    // 读定值返回
//   kFaultUpload = 0x2E,        // 故障报告上送
//   kAllYxUpload = 0x3C,        // 全YX上送
//   kAllYcUpload_0 = 0x3D,      // 全YC上送
//   kAllYcUpload_64 = 0x3E,     // 全YC上送
//   kAllYcUpload_128 = 0x3F,    // 全YC上送
//   kAllYcUpload_192 = 0x40,    // 全YC上送
//   kAllYcUpload_256 = 0x41,    // 全YC上送
//   kChangedYcUpload = 0x42,    // 变化YC上送
//   kReadVersionReturn = 0xA0,  // 读版本信息返回
//   kReadComReturn = 0xA1,      // 读通讯配置返回
//   kWriteComReturn = 0xA2,     // 写通讯配置返回
//   kWriteSingleValue = 0x97,   // 写单个定值
//   kUnkown = 0xff,
// };
//
// // clang-format off
// REFLECT_ENUM(CommandType, int,
//   (kYkSelect, 0x04),
//   (kYkExec ,0x05),
//   (kYkCancel ,0x06),
//   (kUploadValue , 0x07),
//   (kWriteValue ,0x08),
//   (kReadValue ,0x09),
//   (kReset ,0x11),
//   (kReadVersion ,0x80),
//   (kReadCom ,0x81),
//   (kWriteCom ,0x82),
//   (kSOE ,0x22),
//   (kYkSelectReturn ,0x24),
//   (kYkExecReturn ,0x25),
//   (kYkCancelReturn ,0x26),
//   (kUploadValueReturn ,0x27),
//   (kWriteValueReturn ,0x28),
//   (kReadValueReturn ,0x29),
//   (kFaultUpload ,0x2e),
//   (kAllYxUpload ,0x3c),
//   (kAllYcUpload_0 ,0x3d),
//   (kAllYcUpload_64 ,0x3e),
//   (kAllYcUpload_128 ,0x3f),
//   (kAllYcUpload_192 ,0x40),
//   (kAllYcUpload_256 ,0x41),
//   (kChangedYcUpload ,0x42),
//   (kReadVersionReturn ,0xa0),
//   (kReadComReturn ,0xa1),
//   (kWriteComReturn ,0xa2),
//   (kWriteSingleValue ,0x97),
//   (kUnkown ,0xff)
// )
// REFLECT_ENUM(CommandType, std::string,
//   (kYkSelect, "yk-select"),
//   (kYkExec ,"yk-exec"),
//   (kYkCancel ,"yk-cancel"),
//   (kUploadValue ,"upload-value"),
//   (kWriteValue ,"write-value"),
//   (kReadValue ,"read-value"),
//   (kReset ,"reset"),
//   (kReadVersion ,"read-version"),
//   (kReadCom ,"read-com"),
//   (kWriteCom ,"write-com"),
//   (kSOE ,"soe"),
//   (kYkSelectReturn ,"select-return"),
//   (kYkExecReturn ,"yk-exec-return"),
//   (kYkCancelReturn ,"yk-cancel-return"),
//   (kUploadValueReturn ,"upload-value-return"),
//   (kWriteValueReturn ,"write-value-return"),
//   (kReadValueReturn ,"read-value-return"),
//   (kFaultUpload ,"fault-upload"),
//   (kAllYxUpload ,"all-yx-upload"),
//   (kAllYcUpload_0 ,"all-yc-upload-0"),
//   (kAllYcUpload_64 ,"all-yc-upload-64"),
//   (kAllYcUpload_128 ,"all-yc-upload-128"),
//   (kAllYcUpload_192 ,"all-yc-upload-192"),
//   (kAllYcUpload_256 ,"all-yc-upload-256"),
//   (kChangedYcUpload ,"chanhed-yc-upload"),
//   (kReadVersionReturn ,"read-version-return"),
//   (kReadComReturn ,"read-com-return"),
//   (kWriteComReturn ,"write-com-return"),
//   (kWriteSingleValue ,"write-signal-value"),
//   (kUnkown ,"unkown cmd")
// )
// // clang-format on
//
// /**
//  * @brief 宽频的can命令,id的含义抽象
//  */
// class AutonyCanIdDescriptor {
//  public:
//   using IdType = uint32_t;
//
//   inline static AutonyCanIdDescriptor FromId(uint32_t id) {
//     AutonyCanIdDescriptor descriptor;
//     descriptor.id_ = id;
//
//     return descriptor;
//   }
//
//   inline uint32_t GetFrameIndex() const {
//     spiderweb::io::BitmapReader<uint32_t, 0, 5> r(id_);
//     return r.Value();
//   }
//
//   inline void SetFrameIndex(uint8_t index) {
//     spiderweb::io::BitmapWriter<uint32_t, 0, 5> w(id_);
//
//     w.Set(index);
//   }
//
//   inline bool HasMore() const {
//     spiderweb::io::BitmapReader<uint32_t, 6, 6> r(id_);
//     return static_cast<bool>(r.Value());
//   }
//
//   inline void SetHasMore(bool flag) {
//     spiderweb::io::BitmapWriter<uint32_t, 6, 6> w(id_);
//
//     w.Set(flag);
//   }
//
//   inline uint32_t GetAddress() const {
//     spiderweb::io::BitmapReader<uint32_t, 7, 13> r(id_);
//     return r.Value();
//   }
//
//   inline void SetAddress(uint8_t addr) {
//     spiderweb::io::BitmapWriter<uint32_t, 7, 13> w(id_);
//
//     w.Set(addr);
//   }
//
//   // 是否主板主动上送
//   inline bool IsInitiative() const {
//     spiderweb::io::BitmapReader<uint32_t, 16, 19> r(id_);
//     return r.Value() == 4;
//   }
//
//   inline void SetInitiative(bool flag) {
//     spiderweb::io::BitmapWriter<uint32_t, 16, 19> w(id_);
//
//     w.Set(flag ? 4 : 0);
//   }
//
//   inline CommandType GetCommand() const {
//     spiderweb::io::BitmapReader<uint32_t, 20, 27> r(id_);
//
//     return spiderweb::reflect::FromAliasType(r.Value(), CommandType::kUnkown);
//   }
//
//   inline void SetCommand(CommandType cmd) {
//     spiderweb::io::BitmapWriter<uint32_t, 20, 27> w(id_);
//
//     w.Set(static_cast<uint8_t>(cmd));
//   }
//
//   inline void SetExt(uint8_t ext) {
//     spiderweb::io::BitmapWriter<uint32_t, 28, 31> w(id_);
//
//     w.Set(ext);
//   }
//
//   inline uint32_t ToId() const {
//     return id_;
//   }
//
//  private:
//   IdType id_ = 0;
// };
//
// template <CommandType>
// class AutonyCanSubCmd {
//  public:
//   void BuildDataStream(spiderweb::io::DefaultStreamer &s) const;
// };
//
// class AutonyCanFrame {
//  public:
//   AutonyCanFrame() = default;
//
//   inline uint32_t GetFrameCount() const {
//     const auto size = data_stream_.Stream().size();
//
//     return (size % 8 == 0) ? size / 8 : size / 8 + 1;
//   }
//
//   void GetFrame(uint32_t index, spiderweb::serial::CanFrame &output) const {
//     PPK_ASSERT_DEBUG(index >= 0 && index < GetFrameCount(),
//                      "invalid frame index %d, frame count %d", index, GetFrameCount());
//
//     const auto  size = data_stream_.Stream().size();
//     const auto  count = GetFrameCount();
//     const auto *beg = &data_stream_.Stream()[static_cast<std::size_t>(index) * 8];
//     const auto *end = beg + ((size % 8 == 0) ? 8 : size % 8);
//
//     AutonyCanIdDescriptor descriptor;
//
//     descriptor.SetHasMore(count > 1);
//     descriptor.SetFrameIndex(index);
//     descriptor.SetAddress(kDefaultAddress);
//     descriptor.SetCommand(cmd_);
//     descriptor.SetExt(0);
//     descriptor.SetInitiative(false);
//
//     output.header.id(descriptor.ToId());
//     output.header.extended_format(true);
//     output.header.payload_length(end - beg);
//     std::copy(beg, end, std::begin(output.payload));
//   }
//
//   template <CommandType type>
//   inline void BuildFrom(const AutonyCanSubCmd<type> &cmd) {
//     data_stream_.Reset();
//     cmd_ = type;
//     cmd.BuildDataStream(data_stream_);
//   }
//
//  private:
//   CommandType                    cmd_ = CommandType::kUnkown;
//   spiderweb::io::DefaultStreamer data_stream_;
//   const uint8_t                  kDefaultAddress = 1;
// };
//
// template <>
// class AutonyCanSubCmd<CommandType::kWriteSingleValue> {
//  public:
//   inline void SetBlankIndex(uint8_t index) {
//     blank_index_ = index;
//   }
//
//   inline void SetBlankPointNumber(uint16_t point_number) {
//     point_number_ = point_number;
//   }
//
//   inline void SetBlankPointValue(float value, int scale = 1) {
//     value_ = value * scale;
//   }
//
//   void BuildDataStream(spiderweb::io::DefaultStreamer &s) const {
//     using spiderweb::arch::LittleEndian;
//     using spiderweb::io::BinaryWriter;
//     using spiderweb::io::DefaultStreamer;
//
//     BinaryWriter<DefaultStreamer> writer(s);
//
//     writer.Write(LittleEndian<2>(blank_index_), LittleEndian<2>(point_number_),
//                  LittleEndian<2>(value_));
//   }
//
//  private:
//   uint8_t  blank_index_ = 0;
//   uint16_t point_number_ = 0;
//   uint16_t value_ = 0;
// };
//
// template <>
// class AutonyCanSubCmd<CommandType::kWriteValue> {
//  public:
//   inline void SetBlankIndex(uint8_t index) {
//     blank_index_ = index;
//   }
//
//   void BuildDataStream(spiderweb::io::DefaultStreamer &s) const {
//     using spiderweb::arch::LittleEndian;
//     using spiderweb::io::BinaryWriter;
//     using spiderweb::io::DefaultStreamer;
//
//     BinaryWriter<DefaultStreamer> writer(s);
//
//     writer.Write(LittleEndian<2>(blank_index_));
//   }
//
//  private:
//   uint8_t blank_index_ = 0;
// };
//
// TEST(canary, WriteValueId) {
//   spdlog::set_level(spdlog::level::trace);
//   AutonyCanIdDescriptor id = AutonyCanIdDescriptor::FromId(0x10848080);
//
//   const auto cmd_str = spiderweb::reflect::ToAliasType<std::string>(id.GetCommand());
//
//   spdlog::debug("id:{} cmd:{} addr:{} index:{} more:{}", id.ToId(), cmd_str, id.GetAddress(),
//                 id.GetFrameIndex(), id.HasMore());
// }
//
// TEST(canary, WriteSingleValue) {
//   spdlog::set_level(spdlog::level::trace);
//
//   AutonyCanSubCmd<CommandType::kWriteSingleValue> cmd;
//
//   cmd.SetBlankIndex(1);
//   cmd.SetBlankPointNumber(2);
//   cmd.SetBlankPointValue(0);
//
//   AutonyCanFrame frame;
//
//   frame.BuildFrom(cmd);
//
//   EXPECT_EQ(frame.GetFrameCount(), 1);
//
//   spiderweb::serial::CanFrame f;
//   frame.GetFrame(0, f);
//
//   AutonyCanIdDescriptor id = AutonyCanIdDescriptor::FromId(f.header.id());
//
//   const auto cmd_str = spiderweb::reflect::ToAliasType<std::string>(id.GetCommand());
//   spdlog::debug("id:{} cmd:{} addr:{} index:{} more:{} len:{} data:{:n}", f.header.id(), cmd_str,
//                 id.GetAddress(), id.GetFrameIndex(), id.HasMore(), f.header.payload_length(),
//                 spdlog::to_hex(f.payload));
// }
//
// TEST(canary, Id) {
//   AutonyCanIdDescriptor descriptor = AutonyCanIdDescriptor::FromId(0x13c40084);
//   EXPECT_EQ(descriptor.HasMore(), false);
//   EXPECT_EQ(descriptor.GetFrameIndex(), 4);
//   EXPECT_EQ(descriptor.IsInitiative(), 1);
//   EXPECT_TRUE(descriptor.GetCommand() == CommandType::kAllYxUpload);
//   EXPECT_EQ(descriptor.GetAddress(), 1);
// }
//
// TEST(canary, MakeId) {
//   AutonyCanIdDescriptor descriptor;
//
//   descriptor.SetAddress(1);
//   descriptor.SetHasMore(false);
//   descriptor.SetCommand(CommandType::kAllYxUpload);
//   descriptor.SetExt(1);
//   descriptor.SetFrameIndex(4);
//   descriptor.SetInitiative(true);
//
//   EXPECT_EQ(descriptor.ToId(), 0x13c40084) << 0x13c40084;
// }
//
// TEST(canary, Open) {
//   spdlog::set_level(spdlog::level::trace);
//   asio::io_context    ioc;
//   const auto          idx = canary::get_interface_index("can0");
//   auto const          ep = canary::raw::endpoint{idx};
//   canary::raw::socket sock{ioc, ep};
//
//   canary::filter filter;
//   filter.negation(true);
//   filter.remote_transmission(true);
//   filter.extended_format(true);
//
//   sock.set_option(canary::filter_if_any(&filter, 1));
//
//   int i = 0;
//   while (i < 10000) {
//     i++;
//     spiderweb::serial::CanFrame f;
//     sock.receive(canary::net::buffer(&f, sizeof(f)));
//
//     if (!f.header.extended_format()) {
//       continue;
//     }
//
//     AutonyCanIdDescriptor id = AutonyCanIdDescriptor::FromId(f.header.id());
//
//     if (id.GetCommand() != CommandType::kWriteValue &&
//         id.GetCommand() != CommandType::kWriteSingleValue) {
//       continue;
//     }
//
//     const auto cmd_str = spiderweb::reflect::ToAliasType<std::string>(id.GetCommand());
//     spdlog::debug("number:{} id:{} cmd:{} addr:{} index:{} more:{} len:{} data:{:n}", i,
//                   f.header.id(), cmd_str, id.GetAddress(), id.GetFrameIndex(), id.HasMore(),
//                   f.header.payload_length(), spdlog::to_hex(f.payload));
//   }
// }
//
// TEST(canary, WriteSingleValueCan) {
//   spdlog::set_level(spdlog::level::trace);
//   asio::io_context ioc;
//
//   std::error_code     ec;
//   const auto          idx = canary::get_interface_index("can0", ec);
//   auto const          ep = canary::raw::endpoint{idx};
//   canary::raw::socket sock{ioc, ep};
//
//   std::cout << "is open " << sock.is_open() << std::endl;
//   sock.open(canary::raw::socket::protocol_type(), ec);
//
//   canary::filter filter;
//   filter.negation(true);
//   filter.remote_transmission(true);
//   filter.extended_format(true);
//
//   sock.set_option(canary::filter_if_any(&filter, 1));
//   std::cout << "is open " << sock.is_open() << std::endl;
//
//   AutonyCanFrame frame;
//   {
//     AutonyCanSubCmd<CommandType::kWriteSingleValue> cmd;
//
//     cmd.SetBlankIndex(0);
//     cmd.SetBlankPointNumber(22);
//     cmd.SetBlankPointValue(49.9, 100);
//
//     frame.BuildFrom(cmd);
//
//     spiderweb::serial::CanFrame f;
//
//     frame.GetFrame(0, f);
//     sock.send(canary::net::buffer(&f, sizeof(f)));
//   }
//
//   {
//     AutonyCanSubCmd<CommandType::kWriteValue> cmd;
//
//     cmd.SetBlankIndex(0);
//     frame.BuildFrom(cmd);
//     spiderweb::serial::CanFrame f;
//
//     frame.GetFrame(0, f);
//     sock.send(canary::net::buffer(&f, sizeof(f)));
//   }
// }
//
// TEST(canary, AsyncWriteSingleValueCan) {
//   spdlog::set_level(spdlog::level::trace);
//
//   spiderweb::EventLoop         loop;
//   spiderweb::serial::SocketCan can;
//
//   spiderweb::Object::Connect(
//       &can, &spiderweb::serial::SocketCan::Error, &loop,
//       [&](const std::error_code &ec) { std::cout << "error: " << ec.message() << std::endl; });
//
//   spiderweb::Object::Connect(
//       &can, &spiderweb::serial::SocketCan::FrameRead, &loop,
//       [&](const spiderweb::serial::CanFrame &f) {
//         AutonyCanIdDescriptor id = AutonyCanIdDescriptor::FromId(f.header.id());
//
//         const auto cmd_str = spiderweb::reflect::ToAliasType<std::string>(id.GetCommand());
//
//         spdlog::debug("id:{} cmd:{} addr:{} index:{} more:{} len:{} data:{:n}", f.header.id(),
//                       cmd_str, id.GetAddress(), id.GetFrameIndex(), id.HasMore(),
//                       f.header.payload_length(), spdlog::to_hex(f.payload));
//       });
//
//   spiderweb::Object::Connect(&can, &spiderweb::serial::SocketCan::OpenSuccess, &loop, [&]() {
//     std::cout << "open success \n";
//
//     AutonyCanFrame frame;
//
//     {
//       AutonyCanSubCmd<CommandType::kWriteSingleValue> cmd;
//
//       cmd.SetBlankIndex(0);
//       cmd.SetBlankPointNumber(22);
//       cmd.SetBlankPointValue(50.55, 100);
//
//       frame.BuildFrom(cmd);
//
//       spiderweb::serial::CanFrame f;
//
//       frame.GetFrame(0, f);
//
//       can.Write(f);
//     }
//
//     {
//       AutonyCanSubCmd<CommandType::kWriteValue> cmd;
//
//       cmd.SetBlankIndex(0);
//       frame.BuildFrom(cmd);
//
//       spiderweb::serial::CanFrame f;
//
//       frame.GetFrame(0, f);
//       can.Write(f);
//     }
//   });
//
//   spiderweb::Timer timer;
//
//   spiderweb::Object::Connect(&timer, &spiderweb::Timer::timeout, &loop, [&]() { loop.Quit(); });
//
//   timer.SetSingalShot(true);
//   timer.SetInterval(1000 * 10);
//   timer.Start();
//
//   can.Open("can0", spiderweb::serial::SocketCanType::kRaw);
//
//   loop.Exec();
// }
