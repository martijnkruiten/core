import os
import json

from cf_remote import log
from cf_remote.paths import cf_remote_dir
from cf_remote.utils import save_file
import cf_remote.remote as remote


def agent_run(host):
    print("Triggering an agent run on: '{}'".format(host))
    with remote.connect(host) as connection:
        command = "/var/cfengine/bin/cf-agent -Kf update.cf"
        output = remote.ssh_sudo(connection, command)
        log.debug(output)
        command = "/var/cfengine/bin/cf-agent -K"
        output = remote.ssh_sudo(connection, command)
        log.debug(output)


def disable_password_dialog(host):
    print("Disabling password change on hub: '{}'".format(host))
    api = "https://{}/api/user/admin".format(host)
    d = json.dumps({"password": "password"})
    creds = "admin:admin"
    c = "curl -X POST  -k {} -u {}  -H 'Content-Type: application/json' -d '{}'".format(
        api, creds, d)
    log.debug(c)
    os.system(c)


def def_json():
    return {
        "classes": {
            "client_initiated_reporting_enabled": ["any"],
            "mpf_augments_control_enabled": ["any"],
            "services_autorun": ["any"],
            "cfengine_internal_purge_policies": ["any"]
        },
        "vars": {
            "control_server_call_collect_interval": "1",
            "control_hub_exclude_hosts": ["0.0.0.0/0"],
            "mpf_access_rules_collect_calls_admit_ips": ["0.0.0.0/0"],
            "acl": ["0.0.0.0/0", "::/0"],
            "default_data_select_host_monitoring_include": [".*"],
            "default_data_select_policy_hub_monitoring_include": [".*"],
            "control_executor_splaytime": "1"
        }
    }


def install_def_json(host):
    print("Transferring def.json to hub: '{}'".format(host))
    data = json.dumps(def_json(), indent=2)
    path = os.path.join(cf_remote_dir("json"), "def.json")
    save_file(path, data)
    remote.scp(path, host)
    with remote.connect(host) as connection:
        connection.sudo("cp def.json /var/cfengine/masterfiles/def.json")
