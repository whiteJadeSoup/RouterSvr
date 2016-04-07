#ifndef M2RMSGTYPEDEFINE_HPP_INCLUDED
#define M2RMSGTYPEDEFINE_HPP_INCLUDED



enum class M2R
{
    /*
     *  不同消息服务器之间转发聊天消息
     *  @parm sendid:   发送者id
     *  @parm recvid:   接受者id
     *  @parm content:  会话内容
     */
    DISPATCH_CHAT            = 2000,
    /*
     *  用户上线
     *  @parm userid:   上线者id
     */
    USER_LOGIN               = 2001,
    // 同上
    USER_LOGOUT              = 2002,
    /*
     *  给msgsvr分配监听端口
     *  @return port:  分配好的端口
     */
    ALLOCATE_PORT            = 2003,

    SEND_CHAT                = 3000,
};


// 数据库服务器与路由服务器消息定义
enum class R2D
{
    /*
     *  添加一条离线消息
     * @parm send_id    : 发送者id
     * @parm recv_id    : 接受者id
     * @parm content    : 聊天内容
     */

    ADD_OFFLINE_MESSAGE     = 4000,
};




#endif // M2RMSGTYPEDEFINE_HPP_INCLUDED
