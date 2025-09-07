#pragma once

#include "PresetPlaylist.h"
#include "notifications/AudioDataAvailable.h"
#include "notifications/PlaybackControl.h"

#include <projectM-4/projectM.h>

#include <Poco/Logger.h>
#include <Poco/NObserver.h>

#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/Util/Subsystem.h>

#include <memory>

class ProjectMWrapper : public Poco::Util::Subsystem
{
public:
    const char* name() const override;

    void initialize(Poco::Util::Application& app) override;

    void uninitialize() override;

    /**
     * Returns the projectM instance handle.
     * @return The projectM instance handle used to call API functions.
     */
    projectm_handle ProjectM() const;

    /**
     * Returns the current playlist.
     * @return The currently active preset playlist.
     */
    const PresetPlaylist& Playlist() const;

    /**
     * Returns the current playlist.
     * @return The currently active preset playlist.
     */
    PresetPlaylist& Playlist();

    /**
     * @brief Detaches the current playlist and loads a single preset.
     * @param presetData The preset data to load.
     * @param errorMessage The error message from projectM if loading failed.
     * @return true if the preset was loaded successfully, false if an error occurred.
     */
    bool LoadPresetData(const std::string& presetData, std::string& errorMessage);

    /**
     * @brief Detaches the internal playlist, so it no longer controls the preset playback.
     */
    void UnbindPlaylist();

    /**
     * @brief Binds the internal playlist and resets the preset lock to the user setting.
     */
    void BindPlaylist();

    /**
     * Renders a single projectM frame.
     */
    void RenderFrame();

    /**
     * @brief Returns the targeted FPS value.
     * @return The user-configured target FPS. Can be 0, which means unlimited.
     */
    int TargetFPS();

    /**
     * @brief Updates projectM with the current, actual FPS value.
     * @param fps The current FPS value.
     */
    void UpdateRealFPS(float fps) const;

    /**
     * @brief If splash is disabled, shows the initial preset.
     * If shuffle is on, a random preset will be picked. Otherwise, the first playlist item is displayed.
     */
    void DisplayInitialPreset();

    /**
     * @brief Changes beat sensitivity by the given value.
     * @param value A positive or negative delta value.
     */
    void ChangeBeatSensitivity(float value) const;

    /**
     * @brief Returns the libprojectM version this application was built against.
     * @return A string with the libprojectM build version.
     */
    static std::string ProjectMBuildVersion();

    /**
     * @brief Returns the libprojectM version this application currently runs with.
     * @return A string with the libprojectM runtime library version.
     */
    static std::string ProjectMRuntimeVersion();

    /**
     * @brief Returns the full path of the currently displayed preset.
     * @return The full path of the currently displayed preset, or an empty string if the idle preset is loaded.
     */
    std::string CurrentPresetFileName() const;

    /**
     * @brief Toggles handling of playback control notifications.
     * @param enable true to enable handling of playback control notifications, false to disable.
     */
    void EnablePlaybackControl(bool enable);

    /**
     * @brief Locks or unlocks the current preset without changing the user setting.
     * @param lock true to lock the current preset, false to enable auto-switching.
     */
    void HardLockPreset(bool lock);

    /**
     * Copies the full path of the current preset into the OS clipboard.
     */
    void PresetFileNameToClipboard() const;

private:
    void PlaybackControlNotificationHandler(const Poco::AutoPtr<Notification::PlaybackControl>& notification);

    void AudioDataAvailableNotificationHandler(const Poco::AutoPtr<Notification::AudioDataAvailable>& notification);

    static void PresetSwitchFailedEvent(const char* presetFilename, const char* message, void* context);

    std::vector<std::string> GetPathListWithDefault(const std::string& baseKey, const std::string& defaultPath);

    /**
     * @brief Event callback if a configuration value has changed.
     * @param property The key and value that has been changed.
     */
    void OnConfigurationPropertyChanged(const Poco::Util::AbstractConfiguration::KeyValue& property);

    /**
     * @brief Event callback if a configuration value has been removed.
     * @param key The key of the removed property.
     */
    void OnConfigurationPropertyRemoved(const std::string& key);

    Poco::AutoPtr<Poco::Util::AbstractConfiguration> _userConfig; //!< View of the "projectM" configuration subkey in the "user" configuration.
    Poco::AutoPtr<Poco::Util::AbstractConfiguration> _projectMConfigView; //!< View of the "projectM" configuration subkey in the "effective" configuration.

    projectm_handle _projectM{nullptr}; //!< Pointer to the projectM instance used by the application.
    std::unique_ptr<PresetPlaylist> _playlist; //!< The currently active playlist.
    bool _playbackControlEnabled{true}; //!< If false, any playback control notifications are ignored.

    bool _presetLoadFailed{false};
    std::string _presetLoadFailedMessage;

    uint32_t _audioChannels{0}; //!< Number of audio channels of the current capture device.
    std::vector<float> _audioStagingBuffer; //!< Buffer which receives audio data from the capture implementation.
    mutable Poco::Mutex _audioBufferMutex; //!< Mutex protecting access to the audio staging buffer.

    Poco::NObserver<ProjectMWrapper, Notification::PlaybackControl> _playbackControlNotificationObserver{*this, &ProjectMWrapper::PlaybackControlNotificationHandler};
    Poco::NObserver<ProjectMWrapper, Notification::AudioDataAvailable> _audioDataAvailableNotificationObserver{*this, &ProjectMWrapper::AudioDataAvailableNotificationHandler};

    Poco::Logger& _logger{Poco::Logger::get("SDLRenderingWindow")}; //!< The class logger.
};
