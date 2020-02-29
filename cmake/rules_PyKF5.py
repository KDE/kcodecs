# SPDX-FileCopyrightText: 2016 Stephen Kelly <steveire@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause

import os, sys

import rules_engine
sys.path.append(os.path.dirname(os.path.dirname(rules_engine.__file__)))
import Qt5Ruleset

def local_container_rules():
    return [
        ["KCodecs", "Encoder", ".*", ".*", ".*", rules_engine.container_mark_abstract],
        ["KCodecs", "Decoder", ".*", ".*", ".*", rules_engine.container_mark_abstract],
        ["KCodecs", "Codec", ".*", ".*", ".*", rules_engine.container_mark_abstract],
    ]

def local_function_rules():
    return [

        ["KCodecs::Codec", "encode", ".*", ".*", ".*char.*&.*", rules_engine.function_discard],
        ["KCodecs::Codec", "decode", ".*", ".*", ".*char.*&.*", rules_engine.function_discard],

        ["KCodecs::Encoder", "encode", ".*", ".*", ".*char.*&.*", rules_engine.function_discard],
        ["KCodecs::Encoder", "finish", ".*", ".*", ".*char.*&.*", rules_engine.function_discard],
        ["KCodecs::Encoder", "write", ".*", ".*", ".*char.*&.*", rules_engine.function_discard],
        ["KCodecs::Encoder", "writeCRLF", ".*", ".*", ".*char.*&.*", rules_engine.function_discard],
        ["KCodecs::Encoder", "flushOutputBuffer", ".*", ".*", ".*char.*&.*", rules_engine.function_discard],

        ["KCodecs::Decoder", "decode", ".*", ".*", ".*char.*&.*", rules_engine.function_discard],
        ["KCodecs::Decoder", "finish", ".*", ".*", ".*char.*&.*", rules_engine.function_discard],

        ["KCharsets", "codecForName", ".*", ".*", ".*bool &.*", rules_engine.function_discard],
        ["KCharsets", "fromEntity", ".*", ".*", ".*int &.*", rules_engine.function_discard],
    ]

class RuleSet(Qt5Ruleset.RuleSet):
    def __init__(self):
        Qt5Ruleset.RuleSet.__init__(self)
        self._fn_db = rules_engine.FunctionRuleDb(lambda: local_function_rules() + Qt5Ruleset.function_rules())
        self._container_db = rules_engine.ContainerRuleDb(lambda: local_container_rules() + Qt5Ruleset.container_rules())
