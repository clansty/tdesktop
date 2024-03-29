# This file is part of Telegram Desktop,
# the official desktop application for the Telegram messaging service.
#
# For license and copyright information please follow this link:
# https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL

function(generate_appdata_changelog target_name changelog_path appdata_path)
    find_package(Python3 REQUIRED)

    set(gen_dst ${CMAKE_CURRENT_BINARY_DIR}/gen)
    file(MAKE_DIRECTORY ${gen_dst})

    set(gen_timestamp ${gen_dst}/${target_name}_appdata_changelog.timestamp)
    set(gen_files ${appdata_path})

    generate_target(${target_name} appdata_changelog ${gen_timestamp} "${gen_files}" ${gen_dst})
endfunction()
