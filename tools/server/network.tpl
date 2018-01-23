% rebase('base.tpl')

<div>
    <h1>Network</h1>
    % if 'glob' not in hide:
        <div class="log">
        glob<a class="close" href="/network?hide={{','.join(hide + ['glob'])}}">X</a>
        <hr>
        % for g in glob:
            {{g.replace('\1', ' / ')}}<br>
        %end
        </div>
    % end

    % for c in cli:
        % if str(c) not in hide:
            <div class="log">
            {{c}}<a class="close" href="/network?hide={{','.join(hide + [str(c)])}}">X</a>
            <hr>
            % for l in cli[c]:
                {{l.replace('\1', ' / ')}}<br>
            %end
            </div>
        % end
    % end
</div>

