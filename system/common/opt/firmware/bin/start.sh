#!/bin/sh

firmware_dir="$( dirname "$( dirname "$( readlink -f "$0" )" )" )"
hadir=$firmware_dir/bin
data_dir=$firmware_dir/data
scripts_dir=$firmware_dir/scripts
addons_dir=$hadir/addons
services_dir=$firmware_dir/services
log_dir="$( dirname "$( dirname "$firmware_dir" )" )/var/log/firmware"

while [ $# -gt 0 ]
do
    case "$1" in
        -l|--logdir) log_dir="$2";  shift 2 ;;
        -d|--daemon) daemon="$2";   shift 2 ;;
        *)
            echo "$1 is not an option"
            exit 1
            ;;
    esac
done

init () {
  mkdir -p "$log_dir"
  mkdir -p "$scripts_dir"
  mkdir -p "$addons_dir"

  if [ -L "$data_dir" ]
  then
    mkdir -p /home/data
  else
    # for local use
    mkdir -p "$data_dir"
  fi

  if [ ! -d "$data_dir/custom_plugins" ]
  then
    mkdir -p "$data_dir/custom_plugins"
  fi

  if [ -e /home/.factorytest_required ];
  then
    echo ".factorytest_required flag detected - firmware daemons will not start!" > /dev/kmsg
    exit 0
  fi

  echo "Check if there are pending updates."
  "$hadir"/updatecmd

  # Migrate data if needed
  "$hadir"/migration_tool >> "$log_dir/migration.log" 2>&1

  # Schedule logrotate
  ( "$hadir"/logcmd --update-settings 1> /dev/null )&


  cd "$hadir/addons" || return
  for file in $(find . -maxdepth 1 -type f ! -name "*.*" -name "ha-*" ! -name "ha-*cmd"); do
    if [ ! -f "$hadir/${file}.sh" ];
    then
      ln -sf "$hadir/hz-daemon.sh" "$hadir/${file}.sh";
    fi
  done

  cd "$hadir" || return
  for file in $(find . -maxdepth 1 -type f ! -name "*.*" -name "ha-*" ! -name "ha-*cmd"); do
    if [ ! -f "$hadir/${file}.sh" ];
    then
      ln -sf "$hadir/hz-daemon.sh" "$hadir/${file}.sh";
    fi
  done

  for service in $(find "$services_dir" -mindepth 2 -maxdepth 2 -type f ! -name "*.*" -name "ha-*" ! -name "ha-*cmd"); do
    file="$(basename "$service")"
    if [ ! -f "$hadir/${file}.sh" ];
    then
      ln -sf "$hadir/hz-daemon.sh" "$hadir/${file}.sh";
    fi
  done
}

#start daemon script
start_ds () {
  pid="$(pgrep -f "$1")"
  printname="$(basename "$1")"
  if [ -n "$pid" ]
  then
    echo "Skipping $printname: it's already running with pid $pid"
    return;
  fi
  echo  "Starting $printname"
  ($1 -l "$log_dir" > /dev/null 2>&1 )&
}

#start daemon
start_addons () {
  for i in "$addons_dir"/ha-*
  do
    script="$hadir/$(basename "$i").sh"
    start_ds "$script"
  done
}

#start services
start_services () {
  for service in $(find "$services_dir" -mindepth 2 -maxdepth 2 -type f ! -name "*.*" -name "ha-*" ! -name "ha-*cmd"); do
    script="$hadir/$(basename "$service").sh"
    start_ds "$script"
  done
}

#start core daemon
start_core_daemons () {
  # NOTE: keep in reverse order with stop.sh / kill_core_daemons
  start_ds "$hadir/ha-backupd.sh"
  start_ds "$hadir/ha-cored.sh"
  start_ds "$hadir/ha-hardware_manager.sh"
  start_ds "$hadir/ha-networkd.sh"
  start_ds "$hadir/ha-uid.sh"
  start_ds "$hadir/ha-update_manager.sh"
  start_ds "$hadir/ha-cloud_devices.sh"
  start_ds "$hadir/ha-scenes_manager.sh"
  start_ds "$hadir/ha-extensions_manager.sh"
  start_ds "$hadir/ha-external_controllers.sh"
}

#start specific addon/service if not already started
start_specific_component () {
  if { [ -f "$addons_dir/$1" ] || [ -f "$hadir/$1" ] ; } && [ -f "$hadir/$1.sh" ];
  then
    start_ds "$hadir/$1.sh"
  fi
}

#start migration tool first was done here

if [ -z "$daemon" ]
then
  test -x /usr/sbin/led_cmd && /usr/sbin/led_cmd --take-control
  init
#start all
#check if spread is running and start if it is not
  pid_spread="$(pgrep spread)"
  if [ -n "$pid_spread" ]
  then
    echo "Skipping spread.sh: it's already running with pid $pid_spread"
  else
    echo  "Starting spread.sh"
    ("$hadir"/spread.sh -l "$log_dir" >> "$log_dir/spread.log" 2>&1 )&
    echo  "Starting spread_watchdog.sh"
    ("$hadir"/spread_watchdog.sh >> "$log_dir/spread_watchdog.log" 2>&1 )&
  fi
  sleep 1
#start all daemons in predefined sequence
  start_core_daemons
  sleep 1
  start_addons
  sleep 1
  start_services
else
  start_specific_component "$daemon"
fi

exit 0
