// Copyright (c) 2011 Zeex
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SAMPGDK_CALLBACKS_H
#define SAMPGDK_CALLBACKS_H

#include <deque>
#include <map>
#include <string>

#include "amx/amx.h"

class CallbackManager {
public:
	static CallbackManager *GetInstance();

	void RegisterCallbackHandler(void *handler);

	void PushArg(cell value);
	void PushArg(cell *value);

	bool HandleCallback(const char *name);

private:
	CallbackManager();

	enum ArgType {
		ARG_VALUE,
		ARG_STRING
	};

	struct Arg {
		cell        value;
		std::string string;
		ArgType     type;
	};

	std::deque<Arg> args_;

	std::map<void*, std::map<std::string, void*> > cache_;
};

#endif