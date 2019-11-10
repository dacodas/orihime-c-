import io

from ruamel.yaml import YAML

class Path():

    def __init__(self, _yaml):

        self._yaml = _yaml
        self._output = io.StringIO()

    def SetEnvIf(self):

        environment_variables = self._yaml.get("env")
        if environment_variables:

            self._output.write('SetEnvIf Request_URI "{}" '.format(self._yaml["path"]))

            envvars_with_index = zip(
                environment_variables,
                range(1, len(environment_variables) + 1))

            formatted_envvars = [
                "{}=${}".format(variable, number)
                for variable, number
                in envvars_with_index
            ]

            joined_envvars = ' '.join(formatted_envvars)

            self._output.write(joined_envvars)
            self._output.write('\n')

    def RewriteRule(self):

        self._output.write(
            'RewriteRule "{}" "{}/%{{REQUEST_METHOD}}" [END]'
            .format(self._yaml["path"],
                    self._yaml["path"]
                    .replace('(.*?)', 'id')
                    .replace('^', '')
                    .replace('/$', '')
                    .replace('$', '')))

        self._output.write('\n')

    def rule(self):

        self.SetEnvIf()
        self.RewriteRule()
        return self._output.getvalue()

class Paths():

    def __init__(self, filepath):

        self._YAML = YAML(typ='safe')

        with open("httpd-conf.yaml", "r") as f:

            self._yaml = self._YAML.load(f)

        self.paths = [Path(path) for path in self._yaml["paths"]]

paths = Paths("httpd-conf.yaml")

for path in paths.paths:

    print(path.rule())
