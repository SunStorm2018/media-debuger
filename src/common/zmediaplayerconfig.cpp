// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "zmediaplayerconfig.h"
#include <QProcess>
#include <QDebug>

ZMediaPlayerConfig* ZMediaPlayerConfig::m_instance = nullptr;

ZMediaPlayerConfig::ZMediaPlayerConfig()
{
    initializePlayerConfigs();
}

ZMediaPlayerConfig::~ZMediaPlayerConfig()
{
}

ZMediaPlayerConfig* ZMediaPlayerConfig::instance()
{
    if (!m_instance) {
        m_instance = new ZMediaPlayerConfig();
    }
    return m_instance;
}

void ZMediaPlayerConfig::initializePlayerConfigs()
{
    // VLC media player
    ZMediaPlayerInfo vlc;
    vlc.name = "VLC Media Player";
    vlc.executable = "vlc";
    vlc.checkCommand = "which vlc";
    vlc.installCommand = "pkexec apt-get install -y vlc";
    vlc.playCommands << "vlc \"%1\"";
    vlc.description = "Powerful cross-platform multimedia player";
    vlc.isInstalled = false;
    m_players[PLAYER_VLC] = vlc;

    // MPV player
    ZMediaPlayerInfo mpv;
    mpv.name = "MPV Player";
    mpv.executable = "mpv";
    mpv.checkCommand = "which mpv";
    mpv.installCommand = "pkexec apt-get install -y mpv";
    mpv.playCommands << "mpv \"%1\"";
    mpv.description = "Simple, high-performance media player";
    mpv.isInstalled = false;
    m_players[PLAYER_MPV] = mpv;

    // FFplay player
    ZMediaPlayerInfo ffplay;
    ffplay.name = "FFplay";
    ffplay.executable = "ffplay";
    ffplay.checkCommand = "which ffplay";
    ffplay.installCommand = "pkexec apt-get install -y ffmpeg";
    ffplay.playCommands << "ffplay \"%1\"";
    ffplay.description = "Simple media player included with FFmpeg";
    ffplay.isInstalled = false;
    m_players[PLAYER_FFPLAY] = ffplay;

    // SMPlayer
    ZMediaPlayerInfo smplayer;
    smplayer.name = "SMPlayer";
    smplayer.executable = "smplayer";
    smplayer.checkCommand = "which smplayer";
    smplayer.installCommand = "pkexec apt-get install -y smplayer";
    smplayer.playCommands << "smplayer \"%1\"";
    smplayer.description = "Feature-rich MPlayer frontend";
    smplayer.isInstalled = false;
    m_players[PLAYER_SMPLAYER] = smplayer;

    // Totem (GNOME Videos)
    ZMediaPlayerInfo totem;
    totem.name = "Totem";
    totem.executable = "totem";
    totem.checkCommand = "which totem";
    totem.installCommand = "pkexec apt-get install -y totem";
    totem.playCommands << "totem \"%1\"";
    totem.description = "Default media player for GNOME desktop environment";
    totem.isInstalled = false;
    m_players[PLAYER_TOTEM] = totem;

    // GNOME MPlayer
    ZMediaPlayerInfo gnomeMPlayer;
    gnomeMPlayer.name = "GNOME MPlayer";
    gnomeMPlayer.executable = "gnome-mplayer";
    gnomeMPlayer.checkCommand = "which gnome-mplayer";
    gnomeMPlayer.installCommand = "pkexec apt-get install -y gnome-mplayer";
    gnomeMPlayer.playCommands << "gnome-mplayer \"%1\"";
    gnomeMPlayer.description = "GNOME-style MPlayer frontend";
    gnomeMPlayer.isInstalled = false;
    m_players[PLAYER_GNOME_MPLAYER] = gnomeMPlayer;

    // KMPlayer
    ZMediaPlayerInfo kmplayer;
    kmplayer.name = "KMPlayer";
    kmplayer.executable = "kmplayer";
    kmplayer.checkCommand = "which kmplayer";
    kmplayer.installCommand = "pkexec apt-get install -y kmplayer";
    kmplayer.playCommands << "kmplayer \"%1\"";
    kmplayer.description = "Media player for KDE desktop environment";
    kmplayer.isInstalled = false;
    m_players[PLAYER_KMPLAYER] = kmplayer;

    // Xine
    ZMediaPlayerInfo xine;
    xine.name = "Xine";
    xine.executable = "xine";
    xine.checkCommand = "which xine";
    xine.installCommand = "pkexec apt-get install -y xine";
    xine.playCommands << "xine \"%1\"";
    xine.description = "Classic Unix media player";
    xine.isInstalled = false;
    m_players[PLAYER_XINE] = xine;

    // Check installation status of all players during initialization
    for (auto& player : m_players) {
        player.isInstalled = isPlayerInstalled(m_players.key(player));
    }
}

QMap<QString, ZMediaPlayerInfo> ZMediaPlayerConfig::getAllPlayers() const
{
    return m_players;
}

ZMediaPlayerInfo ZMediaPlayerConfig::getPlayerInfo(const QString& playerKey) const
{
    return m_players.value(playerKey);
}

bool ZMediaPlayerConfig::isPlayerInstalled(const QString& playerKey)
{
    if (!m_players.contains(playerKey)) {
        return false;
    }

    QProcess process;
    process.start("bash", QStringList() << "-c" << m_players[playerKey].checkCommand);
    process.waitForFinished(3000);
    
    bool installed = (process.exitCode() == 0);
    m_players[playerKey].isInstalled = installed;
    
    return installed;
}

QString ZMediaPlayerConfig::getInstallCommand(const QString& playerKey) const
{
    if (!m_players.contains(playerKey)) {
        return QString();
    }
    
    return m_players[playerKey].installCommand;
}

QStringList ZMediaPlayerConfig::getPlayCommands(const QString& playerKey, const QString& filePath) const
{
    if (!m_players.contains(playerKey)) {
        return QStringList();
    }
    
    QStringList commands;
    for (const QString& templateCmd : m_players[playerKey].playCommands) {
        commands << templateCmd.arg(filePath);
    }
    
    return commands;
}