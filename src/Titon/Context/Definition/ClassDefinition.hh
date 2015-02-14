<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use ReflectionClass;
use ReflectionMethod;
use Titon\Context\Depository;

class ClassDefinition extends Definition implements DefinitionInterface
{
    protected string $class;

    protected array $methods = [];

    public function __construct(string $alias, mixed $class, Depository $depository)
    {
        parent::__construct($alias, $depository);

        $this->class = $class;
    }

    public function create(...$arguments)
    {
        $reflection = new ReflectionClass($this->class);
        $object = $reflection->newInstanceArgs(
            $this->resolveArguments(...$arguments)
        );

        return $this->callMethods($object);
    }

    public function call(string $method, ...$arguments): this
    {
        $this->methods[] = [
            'method' => $method,
            'arguments' => $arguments,
        ];

        return $this;
    }

    protected function callMethods($object)
    {
        foreach ($this->methods as $method) {
            $reflection = new ReflectionMethod($object, $method['method']);

            foreach ($method['arguments'] as $arg) {
                $args[] = (class_exists($arg)) ? $this->depository[$arg] : $arg;
            }

            $reflection->invokeArgs($object, $args);
        }

        return $object;
    }
}