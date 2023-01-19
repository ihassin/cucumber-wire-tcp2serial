require 'timeout'
require 'socket'
require 'cucumber/wire/data_packet'
require 'cucumber/wire/connection'
require 'cucumber/wire/configuration'
require 'serialport'
require 'json'

# Patch 
class WireConnectionWithSocket < Cucumber::Wire::Connection
    def initialize(socket)
        # FIXME We might want to be able to pass in the timeouts to use
        config = Cucumber::Wire::Configuration.new({ host: socket.remote_address.ip_address, port: socket.remote_address.ip_port })
        super(config)
        @socket = socket
    end

    def recv_packet(handler)
        begin
            response = fetch_data_from_socket(@config.timeout('invoke'))
            puts response.to_json
            reply = response.handle_with(handler)
            if reply
                send_data_to_socket(reply.to_json)
            end
        rescue Timeout::Error => e
            backtrace = e.backtrace ; backtrace.shift # because Timeout puts some wierd stuff in there
            raise Timeout::Error, "Timed out calling wire server with message '#{message}'", backtrace
        end
    end
end

class RequestHandler
    def initialize(serial_port, steps, connection = nil, registry = nil)
        @connection = connection
        @message = underscore(self.class.name.split('::').last)
        @registry = registry
        # Should this be spun out into a class that the
        # user provides, that handles begin/end scenario and matching steps?
        @serial_port = serial_port
        @serial_connection = nil
        @steps = steps
    end

    def execute(request_params = nil)
        @connection.call_remote(self, @message, request_params)
    end

    def handle_fail(params)
        raise @connection.exception(params)
    end

    def handle_step_matches(params)
        puts "handle_step_matches"

        matched = nil
        matched_step_id = nil
        name_to_match = params['name_to_match']

        @steps.keys.each do |k|
            # The step might have parameters
            sqre = k.to_s.gsub("{string}", "'(.+)'")
            dqre = k.to_s.gsub("{string}", "\"(.+)\"")
            matched = (name_to_match.match(sqre) or name_to_match.match(dqre))
            # This won't cope with steps defined with both single- and double-quotes
            # but folk shouldn't mix and match them ;-)
            if matched
                matched_step_id = @steps[k]
                break
            end
        end
        if matched
            match_params = { "id": matched_step_id, "args": [] }
            ma = matched.to_a
            # Get rid of the first match, as that's the full string
            ma.shift
            # Now we're left with just the parameters for the step
            ma.each do |arg|
                match_params[:args].push({ "val": arg, "pos": name_to_match.index(arg) })
            end
            puts "Matched step"
            puts match_params
            Cucumber::Wire::DataPacket.new("success", params = [match_params])
        else
            # The request succeeded, but didn't find anything
            Cucumber::Wire::DataPacket.new("success", params = [])
        end
    end

    def handle_begin_scenario(params)
        @serial_connection = SerialPort.open(@serial_port, 38400)
        if @serial_connection
            Cucumber::Wire::DataPacket.new("success", params = [])
        else
            Cucumber::Wire::DataPacket.new("fail", params = [{"message": "Failed to open serial port"}])
        end
    end

    def handle_end_scenario(params)
        @serial_connection.close if @serial_connection
        @serial_connection = nil
        Cucumber::Wire::DataPacket.new("success", params = [])
    end

    def handle_snippet_text(params)
        puts "skipping snippet_text"
        puts params
        # FIXME Include the snippet instructions to add steps here
        Cucumber::Wire::DataPacket.new("success", params = [])
    end

    def handle_invoke(params)
        puts "handle_invoke"
        puts params
        id = params["id"]
        args = params["args"].to_s
        puts "EXEC #{id} #{args}\r"
        @serial_connection.puts "EXEC #{id} #{args}\r"
        resp = @serial_connection.gets.strip
        if resp == "0"
            Cucumber::Wire::DataPacket.new("success", params = [{"message": "Wire does not implement step_snippet"}])
        else
            Cucumber::Wire::DataPacket.new("fail", params = [{"message": "Step failed with #{resp}"}])
        end
    end

    private

    # Props to Rails
    def underscore(camel_cased_word)
        camel_cased_word.to_s.gsub(/::/, '/').
        gsub(/([A-Z]+)([A-Z][a-z])/,'\1_\2').
        gsub(/([a-z\d])([A-Z])/,'\1_\2').
        tr("-", "_").
        downcase
    end
end

# Check we've got the right arguments
if ARGV.length != 2
    puts "Wrong number of arguments"
    puts
    puts "Usage: server.rb <serial port> <listening port>"
    puts
    exit(2)
end

serial_port = ARGV[0]
server_port = ARGV[1].to_i

# Load in the steps
steps_data = File.read('steps.json') or die "Can't find steps.json file"
steps = JSON.parse(steps_data)

# Open listening socket
server = TCPServer.new server_port

request_handler = RequestHandler.new(serial_port, steps)

# Accept new connections
loop do
    client = server.accept
    wc = WireConnectionWithSocket.new(client)

    loop do
        rmsg = wc.recv_packet(request_handler)
    end
end

