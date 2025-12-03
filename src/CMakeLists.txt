set(BOT_SOURCES
   bot/bot.cpp
   bot/musichandler.cpp
   bot/track.cpp
   bot/utils.cpp

   bot/commands/skip_command.cpp
   bot/commands/back_command.cpp
   bot/commands/stop_command.cpp

   bot/commands/history_command.cpp
   bot/commands/queue_command.cpp

   bot/commands/join_command.cpp
   bot/commands/leave_command.cpp

   bot/commands/pause_command.cpp
   bot/commands/resume_command.cpp
   
   bot/commands/play_command.cpp
   bot/commands/ping_command.cpp
)

add_executable(bot main.cpp ${BOT_SOURCES})

# Указываем, где искать заголовочные файлы
target_include_directories(bot PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/bot/
   ${CMAKE_CURRENT_SOURCE_DIR}/bot/commands/
)