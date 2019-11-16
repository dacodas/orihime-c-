import io

from ruamel.yaml import YAML

class Path():

    def __init__(self, _yaml):

        self._yaml = _yaml
        self._output = io.StringIO()

    def CFunctions(self):

        base = self.CFunctionBaseFromPath()
        return ["{}_{}".format(base, method) for method in self._yaml["methods"]]

    def CFunctionBaseFromPath(self):

        return (
            self._yaml["path"]
            .replace('(.+)', 'id')
            .replace('^/', '')
            .replace('/$', '')
            .replace('$', '')
            .replace('/', '_'))

    def SetEnvIfExpr(self):

        output = io.StringIO()

        for method in self._yaml["methods"]:

            output.write('SetEnvIfExpr "%{{REQUEST_URI}} =~ m#{}# && %{{REQUEST_METHOD}} == \'{}\'" FUNCTION={}'
                               .format(self._yaml["path"],
                                       method,
                                       "{}_{}".format(self.CFunctionBaseFromPath(), method)))
            output.write('\n')

        return output.getvalue()

    def SetEnvIf(self):

        output = io.StringIO()
        environment_variables = self._yaml.get("env")

        if environment_variables:

            output.write('SetEnvIf Request_URI "{}" '.format(self._yaml["path"]))

            envvars_with_index = zip(
                environment_variables,
                range(1, len(environment_variables) + 1))

            formatted_envvars = [
                "{}=${}".format(variable, number)
                for variable, number
                in envvars_with_index
            ]

            joined_envvars = ' '.join(formatted_envvars)

            output.write(joined_envvars)
            output.write('\n')

        return output.getvalue()

class Paths():

    def __init__(self, filepath):

        self._YAML = YAML(typ='safe')

        with open("httpd-conf.yaml", "r") as f:

            self._yaml = self._YAML.load(f)

        self.paths = [Path(path) for path in self._yaml["paths"]]

paths = Paths("httpd-conf.yaml")

for path in paths.paths:
    print(path.SetEnvIfExpr())

for path in paths.paths:
    print(path.SetEnvIf())


print(',\n'.join([',\n'.join(['{{"{}", {}}}'.format(function, function) for function in path.CFunctions()])
            for path in paths.paths]))
