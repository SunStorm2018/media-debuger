// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef MEDIAPLAYERCONFIG_H
#define MEDIAPLAYERCONFIG_H

#include <QString>
#include <QStringList>
#include <QMap>

/**
 * @brief Media player configuration information structure
 */
struct ZMediaPlayerInfo {
    QString name;           // Player name
    QString executable;     // Executable file name
    QString checkCommand;   // Command to check if installed
    QString installCommand; // Installation command (using pkexec)
    QStringList playCommands; // Play command template, %1 will be replaced with file path
    QString description;    // Player description
    bool isInstalled;       // Whether installed
    
    // Add equality operator for use in QMap
    bool operator==(const ZMediaPlayerInfo& other) const {
        return name == other.name &&
               executable == other.executable &&
               checkCommand == other.checkCommand &&
               installCommand == other.installCommand &&
               playCommands == other.playCommands &&
               description == other.description &&
               isInstalled == other.isInstalled;
    }
    
    bool operator!=(const ZMediaPlayerInfo& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Media player configuration management class
 */
class ZMediaPlayerConfig
{
public:
    /**
     * @brief Get singleton instance
     * @return ZMediaPlayerConfig instance
     */
    static ZMediaPlayerConfig* instance();

    /**
     * @brief Get all player configurations
     * @return Player information mapping
     */
    QMap<QString, ZMediaPlayerInfo> getAllPlayers() const;

    /**
     * @brief Get specified player information
     * @param playerKey Player key name
     * @return Player information
     */
    ZMediaPlayerInfo getPlayerInfo(const QString& playerKey) const;

    /**
     * @brief Check if player is installed
     * @param playerKey Player key name
     * @return Whether installed
     */
    bool isPlayerInstalled(const QString& playerKey);

    /**
     * @brief Get player's installation command
     * @param playerKey Player key name
     * @return Installation command
     */
    QString getInstallCommand(const QString& playerKey) const;

    /**
     * @brief Get player's play command
     * @param playerKey Player key name
     * @param filePath Media file path
     * @return Play command
     */
    QStringList getPlayCommands(const QString& playerKey, const QString& filePath) const;

private:
    ZMediaPlayerConfig();
    ~ZMediaPlayerConfig();

    void initializePlayerConfigs();

    QMap<QString, ZMediaPlayerInfo> m_players;
    static ZMediaPlayerConfig* m_instance;
};

// Player key name constants
#define PLAYER_VLC "vlc"
#define PLAYER_MPV "mpv"
#define PLAYER_FFPLAY "ffplay"
#define PLAYER_SMPLAYER "smplayer"
#define PLAYER_TOTEM "totem"
#define PLAYER_KMPLAYER "kmplayer"
#define PLAYER_XINE "xine"

#endif // ZMEDIAPLAYERCONFIG_H
