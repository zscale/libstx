#! /bin/bash

# I just started hacking this file, thus, ***incomplete***.

FILE="$1"

if [[ -z "${FILE}" ]]; then
  echo "usage: $0 FILENAME"
  exit 1
fi

_sed() {
  local REGEX="${1}"
  local MSG="${2}"
  test -z "${MSG}" && MSG="${REGEX}"

  echo "[sed] ${MSG}"
  sed -e "${REGEX}" <"${FILE}" >"${FILE}.tmp" || exit 1
  if [[ -f "${FILE}.tmp" ]]; then
    mv -f "${FILE}.tmp" "${FILE}"
  fi
}

echo "*** namespaces ***"
_sed 's/namespace fnord/namespace xzero/g'
_sed 's/fnord::/xzero::/g'
_sed 's/\(using namespace\) fnord/\1 xzero/g'

echo "*** include headers ***"
_sed 's/fnord-\(.*\/\)/xzero-\1/'
_sed 's/^\(.include\) \"\(.*\)\"$/\1 <\2>/'

echo "*** stdtypes ***"
_sed 's/^\(.include .xzero-base\/stdtypes.*\)$//'
_sed 's/\<Vector\>/std::vector/g'
_sed 's/\<String\>/std::string/g'
_sed 's/\<Pair\>/std::pair/g'
_sed 's/\<HashMap\>/std::unordered_map/g'

echo "*** Executor API ***"
_sed 's/^\(.include .xzero-base\/\)thread\/taskscheduler.h/\1executor\/Scheduler.h/'
_sed 's/\<TaskScheduler\>/Scheduler/g'
# ...

echo *** Status ***
_sed 's/^\(.include .xzero-base\/\)status.h/\1Status.h/'
# TODO: Status(eRuntimeError, "...")
# ...

echo "*** Exception Handling ***"
_sed 's/^\(.include xzero-base\/\)exception.h/\1RuntimeError.h/'
_sed 's/\<StandardException\>/RuntimeError/g'
# Exception -> RuntimeError

echo "*** Logging ***"
_sed 's/\<logException\>/logError/g'

echo "*** RefPtr (AutoRef) ***"
_sed 's/^\(.include xzero-base\/\)autoref.h/\1RefPtr.h/'
_sed 's/\<AutoRef\>/RefPtr/g'

echo "*** Option API ***"
_sed 's/^\(.include xzero-base\/\)duration.h/\1TimeSpan.h/'
_sed 's/\<None<[^>]*>()\>/None()/g'

echo "*** Duration API ***"
_sed 's/^\(.include xzero-base\/\)duration.h/\1TimeSpan.h/'
_sed 's/\<Duration\>/TimeSpan/g'

echo "*** Uri API ***"
_sed 's/^\(.include xzero-base\/\)uri.h/\1Uri.h/'
_sed 's/\<URI\>/Uri/g'

echo "*** ieee754 ***"
_sed 's/^\(.include xzero-base\/\)ieee754.h/\1IEEE754.h/'

echo "*** Redis API ***"
_sed 's/^\(.include .xzero-\)base\/net\/redis\/redisconnection.h>/\1redis\/RedisConnection.h>/'
# ...
