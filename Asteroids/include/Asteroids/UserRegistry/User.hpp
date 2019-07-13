#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Core/Object.h>

namespace Urho3D {
    class Connection;
}

namespace Asteroids {

class ASTEROIDS_PUBLIC_API User
{
public:
    /*!
     * @brief Creates an invalid user. Only exists because Urho3D containers
     * need the default constructor to exist. The GUID will be (uint32_t)-1.
     */
    User();

    /*!
     * @brief Creates a player controlled user and generates a unique ID. This
     * is used by the server when registering new users.
     */
    User(const Urho3D::String& username, Urho3D::Connection* connection);

    /*!
     * @brief Creates a user and sets its GUID (can be either a player controlled
     * user or a non-player controlled user, depending on which range the GUID
     * is in). This is used by the client when receiving a E_USERJOINED event.
     */
    User(const Urho3D::String& username, uint32_t guid);

    /*!
     * @brief Creates a non-player controlled user. Generates a unique ID.
     * Connection will be null.
     */
    User(const Urho3D::String& username);

    const Urho3D::String& GetUsername() const;
    Urho3D::Connection* GetConnection() const;
    uint32_t GetGUID() const;

    bool IsPlayerControlled() const;

private:
    Urho3D::String username_;
    Urho3D::Connection* connection_;
    uint32_t guid_;
};

}
