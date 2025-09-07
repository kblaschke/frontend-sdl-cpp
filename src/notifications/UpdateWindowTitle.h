#pragma once

#include <Poco/Notification.h>

namespace Notification {

/**
 * @brief Informs the application that the window title should be updated.
 */
class UpdateWindowTitle : public Poco::Notification
{
public:
    UpdateWindowTitle() = default;

    explicit UpdateWindowTitle(std::string customTitle)
        : _customTitle(std::move(customTitle))
    {}

    std::string name() const override;

    std::string _customTitle;
};

} // namespace Notification
