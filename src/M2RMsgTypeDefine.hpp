#ifndef M2RMSGTYPEDEFINE_HPP_INCLUDED
#define M2RMSGTYPEDEFINE_HPP_INCLUDED



enum class TypeDefine
{
    /*
     *  不同消息服务器之间转发聊天消息
     *  @parm sendid:   发送者id
     *  @parm recvid:   接受者id
     *  @parm content:  会话内容
     */
    M2R_DispatchChat            = 2000,
    /*
     *  用户上线
     *  @parm userid:   上线者id
     */
    M2R_UserLogin               = 2001,
    // 同上
    M2R_UserLogout              = 2002,
    /*
     *  给msgsvr分配监听端口
     *  @return port:  分配好的端口
     */
    M2R_AllocatePort            = 2003,
};

#endif // M2RMSGTYPEDEFINE_HPP_INCLUDED
